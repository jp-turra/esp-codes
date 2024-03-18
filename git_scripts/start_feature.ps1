# Set to exit on error
$ErrorActionPreference = "Stop"

# Get feature_name argument
$feature_name = $args[0]
$script_dir = $PSScriptRoot
$check_feature_script = "$script_dir\check_feature_name.ps1"

# Call check_feature_script with argument feature_name and get its return code
& $check_feature_script $feature_name

# Call check_feature_name.ps1 and check if return code is 1
if ("$LASTEXITCODE" -eq "1") {
    Write-Output "Invalid feature name. Please provide a valid feature name."
    exit 1
}

# Check if git command is available
if (-not (Get-Command "git" -ErrorAction SilentlyContinue)) {
    Write-Host "Git command not found. Please install Git and try again."
    exit 1
}

# Check if branch "dev" exists
$branch_list = $(git branch -a --no-color | Select-String -Pattern "dev")
if (-not $branch_list) {
    Write-Output "Branch 'dev' not found. Please create it and try again."
    exit 1
}

Write-Output "git checkout -b $feature_name dev"
Start-Sleep -s 1
git checkout -b $feature_name dev

# Check if there is any uncommitted changes
if ((git status --porcelain) -ne "") {
    Write-Output "There are uncommitted changes. Skipping pushing feature branch to remote. You must do it manually."
    exit 0
}

Write-Output "git push -u origin $feature_name"
Start-Sleep -s 1
git push -u origin $feature_name

Write-Output "Starting feature $feature_name DONE!"