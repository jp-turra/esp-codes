# Get feature_name argument
$feature_name = $args[0]

$feature_name_pattern = '^([A-Za-z0-9]+\-?)+$'

# Check if feature_name argument is provided
if (-not $feature_name) {
    Write-Host "Feature name not provided. Please provide the feature name as an argument."
    exit 1
}

# Check if feature_name argument matches the pattern
if ($feature_name -match $feature_name_pattern) 
{
    Write-Output "Name: $feature_name"
    exit 0
}
else
{
    Write-Host "Invalid feature name. Please provide a valid feature name."
    exit 1
}