# Set to exit on error
$ErrorActionPreference = "Stop"

# Get feature_name argument
$feature_name = $args[0]
$script_dir = $PSScriptRoot

# Call check_feature_name.ps1 and check if return code is 1
if ("$script_dir\check_feature_name.ps1 $feature_name") {
    exit 1
}

# Check if git command is available
if (-not (Get-Command "git" -ErrorAction SilentlyContinue)) {
    Write-Host "Git command not found. Please install Git and try again."
    exit 1
}

# Check if branch "dev" exists
if (-not (git show-ref --verify --quiet refs/heads/dev)) {
    Write-Output "Branch 'dev' not found. Please create it and try again."
    exit 1
}

# Check if there is any uncommitted changes
if ((git status --porcelain) -ne "") {
    Write-Output "There are uncommitted changes. Stash them before pulling from 'dev'"
    exit 0
}

Write-Output 'git checkout $feature_name'
Start-Sleep -s 1
git checkout $feature_name

Write-Output 'git rebase -i $(git merge-base $feature_name dev)...'
Start-Sleep -s 2
git rebase -i $(git merge-base $feature_name dev)

Write-Output 'git fetch dev'
Start-Sleep -s 1
git fetch dev

Write-Output 'git merge dev'
Start-Sleep -s 1
git merge dev

Write-Output 'Pulling from dev to feature DONE!'