# Set to exit on error
$ErrorActionPreference = "Stop"

# Get feature_name argument
$version = $args[0]
$tag_message = $args[1]
$script_dir = $PSScriptRoot

# Call check_version.ps1 and check if return code is 1
if ("$script_dir\check_version.ps1 $version") {
    exit 1
}

# Check if git command is available
if (-not (Get-Command "git" -ErrorAction SilentlyContinue)) {
    Write-Host "Git command not found. Please install Git and try again."
    exit 1
}

# Check if tag_message is empty
if ("$tag_message" -eq "") {
    Write-Output "Tag message cannot be empty"
    exit 1
}

# Check if branch "dev" exists
if (-not (git show-ref --verify --quiet refs/heads/dev)) {
    Write-Output "Branch 'dev' not found."
    exit 1
}

# Check if branch 'prod' exists
if (-not (git show-ref --verify --quiet refs/heads/prod)) {
    Write-Output "Branch 'prod' not found."
    exit 1
}

Write-Output "git checkout dev"
Start-Sleep -s 1
git checkout dev

Write-Output "git merge --no-ff dev"
Start-Sleep -s 2
git merge --no-ff dev

Write-Output "git tag -a $version -m $tag_message"
Start-Sleep -s 2
git tag -a $version -m $tag_message

Write-Output "git push origin $version"
Start-Sleep -s 2
git push origin $version