# Set to exit on error
$ErrorActionPreference = "Stop"

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
$branch_list = $(git branch -a --no-color | Select-String -Pattern "dev$")
if (-not $branch_list) {
    Write-Output "Branch 'dev' not found. Please create it and try again."
    exit 1
}

# Check if there is any uncommitted changes
if ((git status --porcelain) -ne "") {
    Write-Output "There are uncommitted changes. Stash them before pulling from 'dev'"
    exit 0
}

Write-Output 'git checkout $feature_name'
git checkout $feature_name

Write-Output 'git rebase -i $(git merge-base $feature_name dev)'
Write-Output "Escolha o que deseja fazer com cada commit seguindo as orientações abaixo dos commits! Aguarde 6 segundos para prosseguir."
Start-Sleep -s 6
git rebase -i $(git merge-base $feature_name dev)

Write-Output 'git fetch origin dev'
Start-Sleep -s 1
git fetch origin dev

Write-Output 'git rebase -i dev'
Start-Sleep -s 1
git rebase -i dev

Write-Output 'git push origin --force-with-lease'
Start-Sleep -s 1
git push origin --force-with-lease

Write-Output "git checkout dev"
Start-Sleep -s 1
git checkout dev

Write-Output "git merge $feature_name"
Start-Sleep -s 1
git merge $feature_name

Write-Output "git push"
Start-Sleep -s 1
git push
