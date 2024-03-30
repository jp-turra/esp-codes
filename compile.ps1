$app_name = $args[0]
$port = $args[1]

$script_dir = $PSScriptRoot

# Function to check if return code is 1
function check_return_code {
    if ("$LASTEXITCODE" -eq "1") {
        Write-Output "Error on $args[0]"
        exit 1
    }
}

# Check if --help or -h argument is provided
if ($args -contains "--help" -or $args -contains "-h") 
{
    Write-Output "Usage: .\compile.ps1 [app_name] [port]"
    Write-Output ""
    Write-Output "Arguments:"
    Write-Output "  app_name: The name of the app to compile."
    Write-Output "  port: The serial port to use for the ESP32. If not provided, it will not upload the app to the ESP32."
    Write-Output ""
    exit 0
}

# Check if app_name argument is provided
if (-not $app_name) 
{
    Write-Host "App name not provided. Please provide the app name as an argument."
    exit 1
}


# Check if idf.py command is available
if (-not (Get-Command "idf.py" -ErrorAction SilentlyContinue)) 
{
    # Check if $script_dir/source_esp_idf.ps1 exists
    if (Test-Path "$script_dir\source_esp_idf.ps1") 
    {
        Write-Output "Sourcing source_esp_idf.ps1"
        & "$script_dir\source_esp_idf.ps1"
    }
    else 
    {
        Write-Host "idf.py command not found. Please install ESP-IDF and try again."
        exit 1
    }
}


# Check if folder $script_dir/$app_name exists
if (-not (Test-Path "$script_dir\$app_name")) 
{
    Write-Host "Folder '$app_name' not found. Please check the app name and try again."
    exit 1
}

# Build the application
& idf.py build -C $script_dir/$app_name
check_return_code


# Upload the application
if ($port) 
{
    Write-Output "****************************************************"
    Write-Output ""
    Write-Output "Uploading $app_name to $port in 5 seconds..."
    Write-Output ""
    Write-Output "****************************************************"
    
    Start-Sleep -s 5
    & idf.py -p $port -C $script_dir/$app_name flash
    check_return_code
}