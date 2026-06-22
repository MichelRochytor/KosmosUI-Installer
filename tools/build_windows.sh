#!/bin/bash
# tools/build_windows_setup.sh

# 🌟 Captura o nome do projeto passado dinamicamente pela CLI
PROJETO_NOME=$1
VERSION="1.0.0"

# Valida se o nome foi fornecido para não quebrar o script
if [ -z "$PROJETO_NOME" ]; then
    echo "❌ Erro interno: O nome do projeto não foi passado para o gerador Windows."
    exit 1
fi

echo "======================================================="
echo "🪟 [WINDOWS] Iniciando compilação do Setup .exe para: $PROJETO_NOME"
echo "======================================================="

# Caminho do arquivo de configuração temporário do Inno Setup
ISS_FILE="./output/installer_config.iss"

# 1. Escreve o script do Inno Setup adaptado dinamicamente para o nome do projeto
# Nota: Usamos duas barras invertidas (\\) porque o Inno Setup lê no padrão Windows de caminhos.
cat << EOF > $ISS_FILE
[Setup]
AppName=${PROJETO_NOME}
AppVersion=${VERSION}
DefaultDirName={pf}\\${PROJETO_NOME}
DefaultGroupName=${PROJETO_NOME}
OutputDir=..\\dist
OutputBaseFilename=${PROJETO_NOME}_Setup_v${VERSION}
Compression=lzma
SolidCompression=yes
DisableProgramGroupPage=yes

[Files]
; Copia o binário principal do Windows que foi compilado pelo GCC
Source: "..\\\\output\\\\${PROJETO_NOME}.exe"; DestDir: "{app}"; Flags: ignoreversion
; Copia a pasta resource inteira (imagens, fontes, etc.) mantendo a árvore de diretórios
Source: "..\\\\resource\\\\*"; DestDir: "{app}\\\\resource"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
; Cria o atalho no Menu Iniciar do Windows
Name: "{group}\\\\${PROJETO_NOME}"; Filename: "{app}\\\\${PROJETO_NOME}.exe"
; Cria o atalho opcional na Área de Trabalho (Desktop)
Name: "{commondesktop}\\\\${PROJETO_NOME}"; Filename: "{app}\\\\${PROJETO_NOME}.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Criar um atalho na Área de Trabalho"; GroupDescription: "Ícones Adicionais:"

[Run]
; Opção de "Executar o aplicativo agora" assim que o instalador fechar
Filename: "{app}\\\\${PROJETO_NOME}.exe"; Description: "Lançar ${PROJETO_NOME} agora"; Flags: nowait postinstall skipifsilent
EOF

echo "🔨 Invocando o compilador do Inno Setup (ISCC.exe) via Wine..."

# 2. Executa o compilador nativo do Inno Setup instalado no Wine
# Mapeamos o caminho usando o drive virtual z: do Wine para ele achar o arquivo .iss do Linux
wine "C:\\Program Files (x86)\\Inno Setup 6\\ISCC.exe" "z:\$(pwd)/output/installer_config.iss"

if [ $? -eq 0 ]; then
    echo "======================================================="
    echo "✅ [GG] Instalador Windows compilado com sucesso absoluto!"
    echo "📍 Localização: ./dist/${PROJETO_NOME}_Setup_v${VERSION}.exe"
    echo "======================================================="
    exit 0
else
    echo "❌ Erro: Falha ao compilar o instalador do Windows via Inno Setup (Wine)."
    exit 1
fi

