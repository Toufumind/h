[CmdletBinding()]
param(
    [string]$ProjectPath = '',
    [string]$UnrealEditorPath = $env:UNREAL_EDITOR_EXE,
    [string]$Map = '/Game/Maps/M_RuntimeValidation',
    [string]$McpHost = 'localhost',
    [int]$McpPort = 9877,
    [int]$EditorReadyTimeoutSeconds = 900,
    [int]$PieReadyTimeoutSeconds = 60,
    [int]$StageTimeoutSeconds = 60,
    [int]$PollIntervalMilliseconds = 500,
    [switch]$NoLaunch,
    [switch]$KeepEditorOpen,
    [switch]$SkipPieStop
)

$ErrorActionPreference = 'Stop'

function Resolve-UnrealEditorPath {
    param([string]$Candidate)

    if ($Candidate -and (Test-Path -LiteralPath $Candidate)) {
        return (Resolve-Path -LiteralPath $Candidate).Path
    }

    $knownPaths = @(
        'D:\Projects\UE\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe',
        'C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe',
        'C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe'
    )

    foreach ($path in $knownPaths) {
        if (Test-Path -LiteralPath $path) {
            return $path
        }
    }

    throw 'UnrealEditor.exe was not found. Pass -UnrealEditorPath or set UNREAL_EDITOR_EXE.'
}

function Test-TcpPort {
    param([string]$HostName, [int]$Port)

    $client = [System.Net.Sockets.TcpClient]::new()
    try {
        $connect = $client.BeginConnect($HostName, $Port, $null, $null)
        if (-not $connect.AsyncWaitHandle.WaitOne(1000, $false)) {
            return $false
        }
        $client.EndConnect($connect)
        return $true
    }
    catch {
        return $false
    }
    finally {
        $client.Close()
    }
}

function Wait-TcpPort {
    param([string]$HostName, [int]$Port, [int]$TimeoutSeconds)

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        if (Test-TcpPort -HostName $HostName -Port $Port) {
            return
        }
        Start-Sleep -Seconds 2
    }

    throw "Timed out waiting for MCP socket ws://$HostName`:$Port."
}

function Get-RemainingSeconds {
    param([datetime]$Deadline)

    $remaining = [int][Math]::Ceiling(($Deadline - (Get-Date)).TotalSeconds)
    if ($remaining -lt 0) {
        return 0
    }
    return $remaining
}

function Invoke-Mcp {
    param(
        [string]$Method,
        [hashtable]$Params = @{},
        [int]$TimeoutSeconds = 60
    )

    $uri = [Uri]::new("ws://$McpHost`:$McpPort")
    $socket = [System.Net.WebSockets.ClientWebSocket]::new()
    $cts = [System.Threading.CancellationTokenSource]::new([TimeSpan]::FromSeconds($TimeoutSeconds))

    try {
        $socket.ConnectAsync($uri, $cts.Token).GetAwaiter().GetResult()
        $request = @{
            jsonrpc = '2.0'
            id = 1
            method = $Method
            params = $Params
        } | ConvertTo-Json -Depth 32 -Compress

        $requestBytes = [System.Text.Encoding]::UTF8.GetBytes($request)
        $requestSegment = [ArraySegment[byte]]::new($requestBytes)
        $socket.SendAsync($requestSegment, [System.Net.WebSockets.WebSocketMessageType]::Text, $true, $cts.Token).GetAwaiter().GetResult()

        $buffer = [byte[]]::new(1048576)
        $builder = [System.Text.StringBuilder]::new()
        do {
            $segment = [ArraySegment[byte]]::new($buffer)
            $response = $socket.ReceiveAsync($segment, $cts.Token).GetAwaiter().GetResult()
            if ($response.MessageType -eq [System.Net.WebSockets.WebSocketMessageType]::Close) {
                break
            }
            [void]$builder.Append([System.Text.Encoding]::UTF8.GetString($buffer, 0, $response.Count))
        } while (-not $response.EndOfMessage)

        $json = $builder.ToString()
        if (-not $json) {
            throw "Empty MCP response for $Method."
        }

        $parsed = $json | ConvertFrom-Json
        if ($parsed.error) {
            throw "MCP $Method failed: $($parsed.error.message)"
        }

        $result = $parsed.result
        if ($result -and ($result.PSObject.Properties.Name -contains 'success') -and -not [bool]$result.success) {
            $details = $result.output
            if (-not $details) { $details = $result.result }
            if (-not $details) { $details = $result.error }
            throw "MCP $Method returned success=false. $details"
        }

        return $result
    }
    finally {
        if ($socket.State -eq [System.Net.WebSockets.WebSocketState]::Open) {
            $closeCts = [System.Threading.CancellationTokenSource]::new([TimeSpan]::FromSeconds(2))
            try {
                $socket.CloseAsync([System.Net.WebSockets.WebSocketCloseStatus]::NormalClosure, 'done', $closeCts.Token).GetAwaiter().GetResult()
            }
            catch {
            }
            finally {
                $closeCts.Dispose()
            }
        }
        $cts.Dispose()
        $socket.Dispose()
    }
}

function Wait-PieState {
    param([bool]$Expected, [int]$TimeoutSeconds)

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        try {
            $status = Invoke-Mcp -Method 'pie_control' -Params @{ action = 'status' } -TimeoutSeconds 15
            if ([bool]$status.isPlaying -eq $Expected) {
                return
            }
        }
        catch {
            Write-Host "PIE status poll did not return cleanly: $($_.Exception.Message)"
        }
        Start-Sleep -Seconds 1
    }

    throw "Timed out waiting for PIE isPlaying=$Expected."
}

function Show-LogMatches {
    param([string]$Filter, [int]$MaxLines = 20)

    $result = Invoke-Mcp -Method 'get_output_log' -Params @{ filter = $Filter; maxLines = $MaxLines } -TimeoutSeconds 30
    foreach ($line in $result.lines) {
        Write-Host ("[{0}/{1}] {2}" -f $line.category, $line.verbosity, $line.message)
    }
}

function Invoke-ValidationDriver {
    param(
        [string[]]$Arguments,
        [int]$TimeoutSeconds = 90
    )

    $pathLiteral = $driverPath | ConvertTo-Json -Compress
    $argsLiteral = $Arguments | ConvertTo-Json -Compress
    if ($Arguments.Count -eq 1) {
        $argsLiteral = "[$argsLiteral]"
    }

    $code = @"
import runpy
import sys

sys.argv = [$pathLiteral] + $argsLiteral
runpy.run_path($pathLiteral, run_name="__main__")
"@

    return Invoke-Mcp -Method 'execute_python' -Params @{ code = $code } -TimeoutSeconds $TimeoutSeconds
}

function Get-PreflightRetryReason {
    param([string]$Message)

    if ($Message -match 'TaskCanceled|已取消|timed out|timeout|Empty MCP response') {
        return "MCP handler timeout while editor is still starting"
    }
    if ($Message -match 'still initializing|Editor world is not available|No ValidationSmokeTestActor|PIE game world is not available') {
        return "editor not ready"
    }
    if ($Message -match 'VALIDATION_SMOKE marker') {
        return "preflight marker not available yet"
    }
    return "preflight retryable error"
}

function ConvertFrom-ValidationOutput {
    param(
        [string]$Output,
        [string]$ExpectedKind = ''
    )

    if (-not $Output) {
        return $null
    }

    $markers = @()
    foreach ($line in ($Output -split "`r?`n")) {
        $index = $line.IndexOf('VALIDATION_SMOKE ')
        if ($index -lt 0) {
            continue
        }

        $json = $line.Substring($index + 'VALIDATION_SMOKE '.Length).Trim()
        try {
            $marker = $json | ConvertFrom-Json
            if (-not $ExpectedKind -or $marker.kind -eq $ExpectedKind) {
                $markers += $marker
            }
        }
        catch {
        }
    }

    if ($markers.Count -eq 0) {
        return $null
    }

    return $markers[$markers.Count - 1]
}

function Invoke-ValidationMarker {
    param(
        [string[]]$Arguments,
        [string]$ExpectedKind
    )

    $result = Invoke-ValidationDriver -Arguments $Arguments
    if ($result.output) {
        Write-Host $result.output
    }

    $marker = ConvertFrom-ValidationOutput -Output $result.output -ExpectedKind $ExpectedKind
    if (-not $marker) {
        throw "Validation driver did not produce expected marker '$ExpectedKind'."
    }

    return $marker
}

function Write-ValidationMarker {
    param(
        [string]$Kind,
        [object]$Snapshot
    )

    $payload = @{
        kind = $Kind
        snapshot = $Snapshot
    } | ConvertTo-Json -Depth 16 -Compress
    Write-Host "VALIDATION_SMOKE $payload"
}

function Get-ValidationSnapshot {
    $marker = Invoke-ValidationMarker -Arguments @('--command', 'snapshot') -ExpectedKind 'snapshot'
    return $marker.snapshot
}

function Wait-ValidationSnapshot {
    param(
        [string]$MarkerKind,
        [scriptblock]$Condition,
        [int]$TimeoutSeconds
    )

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    $lastSnapshot = $null
    while ((Get-Date) -lt $deadline) {
        $snapshot = Get-ValidationSnapshot
        $lastSnapshot = $snapshot
        if (& $Condition $snapshot) {
            Write-ValidationMarker -Kind $MarkerKind -Snapshot $snapshot
            return $snapshot
        }
        Start-Sleep -Milliseconds $PollIntervalMilliseconds
    }

    $lastJson = $lastSnapshot | ConvertTo-Json -Depth 16 -Compress
    throw "Timed out waiting for marker '$MarkerKind'. Last snapshot: $lastJson"
}

function Assert-P0FinalSnapshot {
    param([object]$Snapshot)

    $failures = @()
    if ([int]$Snapshot.player_count -ne 1) { $failures += "player_count expected 1, got $($Snapshot.player_count)" }
    if ([int]$Snapshot.enemy_base_count -ne 0) { $failures += "enemy_base_count expected 0, got $($Snapshot.enemy_base_count)" }
    if ($Snapshot.has_shot_instanced_mesh_view -ne $true) { $failures += "has_shot_instanced_mesh_view expected true" }
    if ($Snapshot.found_legacy_projectile_actor -ne $false) { $failures += "found_legacy_projectile_actor expected false" }
    if ([string]$Snapshot.run_phase -ne 'MapSelection') { $failures += "run_phase expected MapSelection, got $($Snapshot.run_phase)" }
    if ([int]$Snapshot.completed_node_count -lt 1) { $failures += "completed_node_count expected >= 1, got $($Snapshot.completed_node_count)" }
    if ([int]$Snapshot.reward_options_count -ne 0) { $failures += "reward_options_count expected 0, got $($Snapshot.reward_options_count)" }

    if ($failures.Count -gt 0) {
        throw "P0 validation failed: $($failures -join '; ')"
    }
}

$repoRoot = Split-Path $PSScriptRoot -Parent
if (-not $ProjectPath) {
    $ProjectPath = Join-Path $repoRoot 'SpriteAssemble.uproject'
}
$driverPath = Join-Path $PSScriptRoot 'ValidationSmokeDriver.py'
$projectFullPath = (Resolve-Path -LiteralPath $ProjectPath).Path
$editorProcess = $null

if (-not (Test-Path -LiteralPath $driverPath)) {
    throw "Missing validation driver: $driverPath"
}

if (-not $NoLaunch) {
    $editorExe = Resolve-UnrealEditorPath -Candidate $UnrealEditorPath
    Write-Host "Launching Unreal Editor: $editorExe"
    $env:NuGetAudit = 'false'
    $env:NUGET_AUDIT = 'false'
    $args = @($projectFullPath, $Map, '-NoSplash')
    $editorProcess = Start-Process -FilePath $editorExe -ArgumentList $args -PassThru
}
else {
    Write-Host "Using existing Unreal Editor process."
}

try {
    $editorReadyDeadline = (Get-Date).AddSeconds($EditorReadyTimeoutSeconds)

    Write-Host "Waiting for MCP socket ws://$McpHost`:$McpPort ..."
    Wait-TcpPort -HostName $McpHost -Port $McpPort -TimeoutSeconds (Get-RemainingSeconds -Deadline $editorReadyDeadline)

    Write-Host "Waiting for editor readiness ..."
    do {
        try {
            $remaining = Get-RemainingSeconds -Deadline $editorReadyDeadline
            if ($remaining -le 0) {
                throw "Timed out waiting for editor readiness after $EditorReadyTimeoutSeconds seconds."
            }

            if ($remaining -lt 10) {
                throw "Not enough editor readiness budget remains for another preflight attempt (${remaining}s)."
            }

            $attemptTimeout = [Math]::Min(90, [Math]::Max(30, $remaining + 15))
            $preflight = Invoke-ValidationDriver -Arguments @('--command', 'preflight', '--map', $Map) -TimeoutSeconds $attemptTimeout
            if ($preflight.output -and $preflight.output.Contains('VALIDATION_SMOKE')) {
                Write-Host $preflight.output
            }
            else {
                throw 'Preflight did not produce a VALIDATION_SMOKE marker yet.'
            }
            break
        }
        catch {
            $remaining = Get-RemainingSeconds -Deadline $editorReadyDeadline
            if ($remaining -le 0) {
                throw "Timed out waiting for editor readiness after $EditorReadyTimeoutSeconds seconds. Last preflight error: $($_.Exception.Message)"
            }
            $reason = Get-PreflightRetryReason -Message $_.Exception.Message
            Write-Host "Editor not ready yet ($reason, ${remaining}s remaining): $($_.Exception.Message)"
            Start-Sleep -Seconds 3
        }
    } while ($true)

    Write-Host "Starting PIE ..."
    try {
        Invoke-Mcp -Method 'pie_control' -Params @{ action = 'start' } -TimeoutSeconds $PieReadyTimeoutSeconds | Out-Null
    }
    catch {
        Write-Host "PIE start request did not return cleanly; polling status: $($_.Exception.Message)"
    }
    Wait-PieState -Expected $true -TimeoutSeconds $PieReadyTimeoutSeconds

    Write-Host "Starting validation flow through ConfigureRewardPool + UICommandController.SelectNode ..."
    $start = Invoke-ValidationMarker -Arguments @('--command', 'start') -ExpectedKind 'start'
    if ($start.reward_pool_configured -ne $true) {
        throw 'RewardPool was not configured by validation actor.'
    }

    Write-Host "Polling until enemy spawn is observed ..."
    $enemySpawned = Wait-ValidationSnapshot -MarkerKind 'enemy_spawned' -TimeoutSeconds $StageTimeoutSeconds -Condition {
        param($snapshot)
        [int]$snapshot.enemy_base_count -ge 2
    }

    Write-Host "Polling until reward options are available before claim ..."
    $rewardAvailable = Wait-ValidationSnapshot -MarkerKind 'reward_available_before_claim' -TimeoutSeconds $StageTimeoutSeconds -Condition {
        param($snapshot)
        ([string]$snapshot.run_phase -eq 'Reward') -and ([int]$snapshot.reward_options_count -gt 0)
    }

    Write-Host "Claiming reward through UICommandController.ClaimReward ..."
    Invoke-ValidationMarker -Arguments @('--command', 'claim') -ExpectedKind 'claim' | Out-Null

    Write-Host "Polling until post-claim map selection state is stable ..."
    $postClaim = Wait-ValidationSnapshot -MarkerKind 'post_claim' -TimeoutSeconds $StageTimeoutSeconds -Condition {
        param($snapshot)
        ([string]$snapshot.run_phase -eq 'MapSelection') -and
            ([int]$snapshot.completed_node_count -ge 1) -and
            ([int]$snapshot.enemy_base_count -eq 0) -and
            ([int]$snapshot.reward_options_count -eq 0)
    }

    Assert-P0FinalSnapshot -Snapshot $postClaim

    Write-Host "Recent validation log lines:"
    Show-LogMatches -Filter 'Validation' -MaxLines 30

    if (-not $SkipPieStop) {
        Write-Host "Stopping PIE ..."
        Invoke-Mcp -Method 'pie_control' -Params @{ action = 'stop' } -TimeoutSeconds 30 | Out-Null
        Start-Sleep -Seconds 2
    }
}
finally {
    if ($editorProcess -and -not $KeepEditorOpen -and -not $editorProcess.HasExited) {
        Write-Host "Closing launched editor process ..."
        $editorProcess.CloseMainWindow() | Out-Null
        if (-not $editorProcess.WaitForExit(10000)) {
            Stop-Process -Id $editorProcess.Id -ErrorAction SilentlyContinue
        }
    }
}
