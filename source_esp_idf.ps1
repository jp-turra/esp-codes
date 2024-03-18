Write-Output "Sourcing source_esp_idf.ps1"

$esp_idf_path = "$env:IDF_PATH"
if (-not $esp_idf_path)
{
    Write-Output "IDF_PATH not set"
    
    exit 1
}

Write-Output "IDF_PATH: $esp_idf_path"

& "$esp_idf_path/export.ps1"