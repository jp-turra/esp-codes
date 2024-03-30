# Set to exit on error
# $ErrorActionPreference = "Stop"

# Get feature_name argument
$version = $args[0]
$tag_message = $args[1]
$script_dir = $PSScriptRoot

function check_return_code {
    if ("$LASTEXITCODE" -eq "1") {
        Write-Output "Error on $args[0]"
        exit 1
    }
}

& "$script_dir\check_version.ps1" $version
# Call check_version.ps1 and check if return code is 1
if ("$LASTEXITCODE" -eq "1") {
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
$branch_list = $(git branch -a --no-color | Select-String -Pattern "dev$")
if (-not $branch_list) {
    Write-Output "Branch 'dev' not found."
    exit 1
}

# Check if branch 'prod' exists
$branch_list = $(git branch -a --no-color | Select-String -Pattern "prod$")
if (-not $branch_list) {
    Write-Output "Branch 'prod' not found."
    exit 1
}

Write-Output "git checkout prod"
Start-Sleep -s 1
git checkout prod
check_return_code "git checkout prod"

Write-Output "git merge --no-ff dev"
Start-Sleep -s 2
git merge --no-ff dev
check_return_code "git merge --no-ff dev"

Write-Output "git tag -a $version -m $tag_message"
Start-Sleep -s 2
git tag -a $version -m $tag_message
check_return_code "git tag -a $version -m $tag_message"

Write-Output "git push origin $version"
Start-Sleep -s 2
git push origin $version
check_return_code "git push origin $version"