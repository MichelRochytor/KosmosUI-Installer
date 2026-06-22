#!/bin/bash
# tools/build_debian.sh

# 🌟 Captura o nome do projeto passado dinamicamente pela CLI
PROJETO_NOME=$1
VERSION="1.0.0"

# Valida se o nome foi fornecido para não quebrar o script
if [ -z "$PROJETO_NOME" ]; then
    echo "❌ Erro interno: O nome do projeto não foi passado para o gerador Debian."
    exit 1
fi

echo "======================================================="
echo "🐧 [DEBIAN] Iniciando empacotamento .deb para: $PROJETO_NOME"
echo "======================================================="

# Define a pasta temporária onde a árvore do pacote será montada
DEB_DIR="./output/debian_pkg"

# 1. Limpa resquícios de builds anteriores e cria a nova árvore nativa Debian
rm -rf $DEB_DIR
mkdir -p $DEB_DIR/DEBIAN
mkdir -p $DEB_DIR/usr/bin
mkdir -p $DEB_DIR/usr/share/applications

echo "📝 Generando arquivo de controle e dependências..."
# 2. Escreve o arquivo de controle essencial do ecossistema APT
# Nota: O nome do pacote no Debian DEVE ser estritamente em letras minúsculas (${PROJETO_NOME,,})
cat << EOF > $DEB_DIR/DEBIAN/control
Package: ${PROJETO_NOME,,}
Version: $VERSION
Architecture: amd64
Maintainer: Michel Rochytor <michel@utfpr.edu.br>
Description: Aplicacao de alto desempenho construida e distribuida via KosmosUI Engine.
Depends: wine, wine64, libc6
EOF

echo "📦 Copiando o binário AppImage consolidado..."
# 3. Localiza o AppImage gerado na etapa anterior do build e joga nos binários do sistema (/usr/bin)
if [ -f "./output/linux/${PROJETO_NOME}-x86_64.AppImage" ]; then
    cp "./output/linux/${PROJETO_NOME}-x86_64.AppImage" "$DEB_DIR/usr/bin/${PROJETO_NOME,,}"
    chmod +x "$DEB_DIR/usr/bin/${PROJETO_NOME,,}"
else
    echo "❌ Erro Crítico: O arquivo AppImage em ./output/linux/ não foi encontrado! Execute o build primeiro."
    exit 1
fi

echo "🎨 Criando atalho para o Menu Iniciar (.desktop)..."
# 4. Cria o lançador para o aplicativo aparecer com ícone no menu do Zorin OS / Ubuntu
cat << EOF > $DEB_DIR/usr/share/applications/${PROJETO_NOME,,}.desktop
[Desktop Entry]
Name=${PROJETO_NOME}
Exec=/usr/bin/${PROJETO_NOME,,}
Icon=utilities-terminal
Type=Application
Categories=Development;Engineering;
Terminal=false
StartupNotify=true
EOF

echo "🔨 Invocando dpkg-deb para consolidar o instalador..."
# 5. Garante a existência da pasta dist/ e roda o utilitário nativo do Linux para buildar o .deb
mkdir -p ./dist
dpkg-deb --build $DEB_DIR "./dist/${PROJETO_NOME,,}_${VERSION}_amd64.deb"

if [ $? -eq 0 ]; then
    echo "======================================================="
    echo "✅ [GG] Pacote Debian gerado com sucesso absoluto!"
    echo "📍 Localização: ./dist/${PROJETO_NOME,,}_${VERSION}_amd64.deb"
    echo "======================================================="
    exit 0
else
    echo "❌ Erro ao executar o dpkg-deb."
    exit 1
fi