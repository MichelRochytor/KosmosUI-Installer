# install.ps1 (Raiz do repositório KOSMOSUI-INSTALLER)

Clear-Host
Write-Host "=======================================================" -ForegroundColor Cyan
Write-Host "  🌌 INSTALADOR GLOBAL DO KOSMOSUI SDK - v1.0.0 (2026) " -ForegroundColor Cyan
Write-Host "=======================================================" -ForegroundColor Cyan
Write-Host "🔧 Iniciando a configuração do ambiente no Windows..." -ForegroundColor Yellow
Write-Host ""

# 1. Validação básica: Verifica se o GCC está disponível para compilar a CLI
$temGcc = Get-Command gcc -ErrorAction SilentlyContinue
if (!$temGcc) {
    Write-Host "⚠️  GCC não encontrado no sistema para compilar a CLI!" -ForegroundColor Yellow
    Write-Host "🔧 O KosmosUI vai tentar instalar o MinGW (GCC) via Windows Package Manager..." -ForegroundColor Cyan
    
    # Invocação segura do winget para adiantar as ferramentas do usuário
    winget install GNU.GCC --silent --accept-package-agreements --accept-source-agreements
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ GCC instalado com sucesso!" -ForegroundColor Green
        Write-Host "👉 IMPORTANTE: Reinicie este terminal e execute o 'install.ps1' novamente para concluir." -ForegroundColor Yellow
        Exit
    } else {
        Write-Host "❌ Erro: Não foi possível instalar o GCC automaticamente via winget." -ForegroundColor Red
        Write-Host "💡 Por favor, instale o MinGW-w64 manualmente e garanta que ele esteja configurado no PATH." -ForegroundColor Cyan
        Exit 1
    }
}

# 2. Compila o utilitário binário da CLI para Windows
Write-Host "🔨 [1/3] Compilando o gerenciador de workspace (kosmos-cli.c)..." -ForegroundColor Cyan
gcc kosmos-cli.c -o kosmos.exe

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Erro Crítico: Falha ao compilar kosmos-cli.c!" -ForegroundColor Red
    Exit 1
}
Write-Host "✅ CLI compilada com sucesso." -ForegroundColor Green

# 3. Cria o diretório de binários locais do usuário (~/.local/bin) se não existir
Write-Host "📁 [2/3] Alocando binário na pasta de execução local do usuário..." -ForegroundColor Cyan
$binPath = "$env:USERPROFILE\.local\bin"
if (!(Test-Path $binPath)) {
    New-Item -ItemType Directory -Force -Path $binPath | Out-Null
}

# Move o executável compilado para a pasta segura do usuário
Move-Item -Path "kosmos.exe" -Destination "$binPath\kosmos.exe" -Force
Write-Host "✅ Binário 'kosmos.exe' instalado em: $binPath" -ForegroundColor Green

# 4. Injeta a pasta de binários locais no PATH do Usuário de forma permanente no Registro
Write-Host "🌟 [3/3] Configurando variáveis de ambiente dinâmicas (PATH)..." -ForegroundColor Cyan
$oldPath = [Environment]::GetEnvironmentVariable("PATH", "User")

if (!$oldPath.Contains(".local\bin")) {
    $newPath = "$oldPath;$binPath"
    [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
    Write-Host "✅ Caminho registrado com sucesso nas suas Variáveis de Ambiente!" -ForegroundColor Green
} else {
    Write-Host "ℹ️  O caminho ~/.local/bin já constava mapeado no seu PATH do Windows." -ForegroundColor Yellow
}

# 5. Conclusão do instalador
Write-Host ""
Write-Host "=======================================================" -ForegroundColor Green
Write-Host "🎉 [GG] KOSMOSUI SDK FOI INSTALADO COM SUCESSO NO WINDOWS!" -ForegroundColor Green
Write-Host "=======================================================" -ForegroundColor Green
Write-Host "👉 IMPORTANTE: Para as alterações entrarem em vigor nativamente,"
Write-Host "   ABRA UM NOVO TERMINAL (CMD ou PowerShell)."
Write-Host ""
Write-Host "Após abrir o novo prompt, digite apenas 'kosmos' para testar! 🔥"
Write-Host "=======================================================" -ForegroundColor Green