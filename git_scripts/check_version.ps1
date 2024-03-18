$version = $args[0]
$version_pattern = "^(\d+\.\d+\.\d+){1}(\-){1}(rc|beta|alpha){1}$"

# Check if feature_name argument matches the pattern
if ($version -match $version_pattern) 
{
    $version_number = $matches[1]
    $version_type = $matches[3]

    Write-Output "Number: $version_number"
    Write-Output "Type: $version_type"
    exit 0
}
else
{
    Write-Host "Invalid version! Please provide a version in the format X.X.X-rc|beta|alpha"
    exit 1
}