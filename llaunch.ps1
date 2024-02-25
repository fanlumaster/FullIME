# generate, compile and run exe files
$currentDirectory = Get-Location
$cmakeListsPath = Join-Path -Path $currentDirectory -ChildPath "CMakeLists.txt"

if (-not (Test-Path $cmakeListsPath))
{
    Write-Host("No CMakeLists.txt in current directory, please check.")
    return
}

Write-Host "Start generating and compiling..."

$buildFolderPath = ".\build"

if (-not (Test-Path $buildFolderPath))
{
    New-Item -ItemType Directory -Path $buildFolderPath | Out-Null
    Write-Host "build folder created."
}

cmake -G "Visual Studio 17 2022" -A x64 -S . -B ./build/

if ($LASTEXITCODE -eq 0)
{
    # DEBUG version
    cmake --build ./build/ --config DEBUG
    # RELEASE version
    # cmake --build ./build/ --config RELEASE
    if ($LASTEXITCODE -eq 0)
    {
        $content = Get-Content -Path "./CMakeLists.txt"
        foreach($line in $content)
        {
            if ($line.StartsWith("add_executable"))
            {
                $pattern = "\((.*?)\)"
                if ($line -match $pattern)
                {
                    $contentInParentheses = $Matches[1]
                    $result = -split $contentInParentheses
                    $exePath = "./build/DEBUG/" + $result[0] + ".exe"
                    Write-Host "start running..."
                    Invoke-Expression $exePath
                }
            }
        }
    }
}
