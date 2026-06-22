#!/bin/bash
# install.sh (Raiz do repositório KOSMOSUI-INSTALLER)

clear
echo "======================================================="
echo "  🌌 INSTALADOR GLOBAL DO KOSMOSUI SDK - v1.0.0 (2026) "
echo "======================================================="
echo "🔧 Iniciando a configuração do ambiente no Zorin OS..."
echo ""

# 1. Validação básica: Verifica se o GCC está instalado no sistema host para compilar a CLI
if ! command -v gcc &> /dev/null; then
    echo "❌ Erro: O compilador 'gcc' não foi encontrado no seu Linux host."
    echo "💡 Por favor, instale o build-essential rodando: sudo apt install build-essential"
    exit 1
fi

# 2. Compila o utilitário binário da CLI do Kosmos
echo "🔨 [1/3] Compilando o gerenciador de workspace (kosmos-cli.c)..."
gcc kosmos-cli.c -o kosmos

if [ $? -ne 0 ]; then
    echo "❌ Erro Crítico: Falha ao compilar kosmos-cli.c!"
    exit 1
fi
echo "✅ CLI compilada com sucesso."

# 3. Cria o diretório de binários locais do usuário (~/.local/bin) se ele não existir
echo "📁 [2/3] Alocando binário na pasta de execução local do usuário..."
mkdir -p "$HOME/.local/bin"

# Move o executável compilado para lá e concede permissões de execução
cp kosmos "$HOME/.local/bin/kosmos"
chmod +x "$HOME/.local/bin/kosmos"

# Remove o binário temporário gerado na raiz do instalador para deixar o repositório limpo
rm kosmos
echo "✅ Binário 'kosmos' instalado com sucesso em: ~/.local/bin/"

# 4. Injeta a pasta de binários locais no PATH do .bashrc caso ela ainda não esteja mapeada
echo "🌟 [3/3] Configurando variáveis de ambiente dinâmicas (.bashrc)..."
if ! grep -q ".local/bin" "$HOME/.bashrc"; then
    echo "" >> "$HOME/.bashrc"
    echo "# Caminho de execução dos binários locais do usuário (KosmosUI CLI)" >> "$HOME/.bashrc"
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
    echo "✅ Linha de exportação adicionada com sucesso ao seu ~/.bashrc!"
else
    echo "ℹ️  O caminho ~/.local/bin já constava mapeado no seu .bashrc. Passo pulado."
fi

# 5. Conclusão do instalador
echo ""
echo "======================================================="
echo "🎉 [GG] KOSMOSUI SDK FOI INSTALADO COM SUCESSO ABSOLUTO!"
echo "======================================================="
echo "👉 IMPORTANTE: Para aplicar a alteração do PATH agora mesmo,"
echo "   execute o comando abaixo ou abra um NOVO terminal:"
echo ""
echo "   source ~/.bashrc"
echo ""
echo "Após fazer isso, digite apenas 'kosmos' para ver os comandos! 🔥"
echo "======================================================="