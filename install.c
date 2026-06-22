// instalar.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// 🌍 URL RAW DO SEU REPOSITÓRIO ONDE MORA O CÓDIGO DA CLI
#define URL_CLI "https://raw.githubusercontent.com/MichelRochytor/KOSMOSUI-INSTALLER/main/kosmos-cli.c"

int main(int argc, char* argv[]) {
#ifdef _WIN32
    system("chcp 65001 > nul"); // Força UTF-8 no Windows para os emojis
#endif

    printf("=======================================================\n");
    printf("🌌 ASSISTENTE SUPREMO DE INSTALAÇÃO - KOSMOSUI SDK     \n");
    printf("=======================================================\n");
    printf("🔧 Preparando ambiente de desenvolvimento multiplataforma...\n\n");

#ifdef _WIN32
    // ================= FLUXO STANDALONE WINDOWS =================
    
    // 1. Garante que o diretório destino exista antes de qualquer operação
    system("if not exist %USERPROFILE%\\.local\\bin mkdir %USERPROFILE%\\.local\\bin");

    // 2. Valida e instala o GCC via Winget se o usuário estiver zerado
    if (system("gcc --version > nul 2>&1") != 0) {
        printf("⚠️  Compilador GCC não detectado no Windows!\n");
        printf("🔧 Baixando e configurando o MinGW-w64 via Winget de forma silenciosa...\n");
        system("winget install GNU.GCC --silent --accept-package-agreements --accept-source-agreements");
    }

    printf("🌐 [1/3] Baixando a CLI estável direto do GitHub Releases...\n");
    // Usa o PowerShell nativo do Windows em background para baixar o arquivo fonte isolado
    char cmdDlWin[1024];
    sprintf(cmdDlWin, "powershell -Command \"Invoke-WebRequest -Uri '%s' -OutFile '%%TEMP%%\\kosmos-cli.c' -Quiet\"", URL_CLI);
    system(cmdDlWin);

    printf("🔨 [2/3] Compilando e otimizando o motor localmente...\n");
    // Compila direto da pasta temporária do sistema para não poluir o disco do usuário
    int rWin = system("gcc %TEMP%\\kosmos-cli.c -o %USERPROFILE%\\.local\\bin\\kosmos.exe");
    system("del /Q %TEMP%\\kosmos-cli.c > nul 2>&1"); // Limpa o rastro de código

    if (rWin != 0) {
        printf("❌ Erro fatal: O GCC instalado não conseguiu compilar a CLI do KosmosUI.\n");
        system("pause");
        return 1;
    }

    printf("🌟 [3/3] Injetando binários nas variáveis de ambiente do usuário (PATH)...\n");
    system("powershell -Command \"$p = [Environment]::GetEnvironmentVariable('PATH', 'User'); if (!$p.Contains('.local\\bin')) { [Environment]::SetEnvironmentVariable('PATH', $p + ';' + $env:USERPROFILE + '\\.local\\bin', 'User') }\"");

    printf("\n=======================================================\n");
    printf("🎉 [GG] KOSMOSUI SDK FOI INSTALADO COM SUCESSO ABSOLUTO!\n");
    printf("=======================================================\n");
    printf("👉 Feche este prompt, abra um NOVO terminal e digite 'kosmos'! 🔥\n");
    system("pause");

#else
    // ================= FLUXO STANDALONE LINUX (ZORIN OS) =================
    
    // Se o usuário clicar duas vezes pelo gerenciador de arquivos do Linux,
    // o binário detecta a ausência de TTY e se auto-injeta dentro de uma janela gráfica do Gnome Terminal
    if (!isatty(STDIN_FILENO)) {
        char cmdTerminal[1024];
        sprintf(cmdTerminal, "gnome-terminal -- \"%s\"", argv[0]);
        system(cmdTerminal);
        return 0;
    }

    if (system("gcc --version > /dev/null 2>&1") != 0) {
        printf("❌ Erro: O pacote 'build-essential' (gcc) não foi encontrado no seu host Linux.\n");
        printf("💡 Por favor, execute primeiro: sudo apt install build-essential\n");
        printf("\nPressione Enter para sair...");
        getchar();
        return 1;
    }

    printf("🌐 [1/3] Buscando árvore estável da CLI via cURL/Wget...\n");
    system("mkdir -p $HOME/.local/bin");
    // Tenta baixar usando o wget ou curl (o que estiver disponível nativamente na distro)
    system("wget -q " URL_CLI " -O /tmp/kosmos-cli.c 2>/dev/null || curl -sL " URL_CLI " -o /tmp/kosmos-cli.c");

    printf("🔨 [2/3] Compilando CLI global em ~/.local/bin/kosmos...\n");
    int rLin = system("gcc /tmp/kosmos-cli.c -o $HOME/.local/bin/kosmos");
    system("rm -f /tmp/kosmos-cli.c"); // Limpeza

    if (rLin != 0) {
        printf("❌ Erro fatal durante a compilação do kosmos-cli.c nativo.\n");
        return 1;
    }
    system("chmod +x $HOME/.local/bin/kosmos");

    printf("🌟 [3/3] Atualizando barramento de caminhos (.bashrc)...\n");
    system("grep -q '.local/bin' $HOME/.bashrc || (echo 'export PATH=\"$HOME/.local/bin:$PATH\"' >> $HOME/.bashrc)");

    printf("\n=======================================================\n");
    printf("🎉 [GG] KOSMOSUI SDK FOI INSTALADO COM SUCESSO NO LINUX!\n");
    printf("=======================================================\n");
    printf("👉 Abra um novo terminal ou digite 'source ~/.bashrc' para ativar o comando 'kosmos'! 🔥\n");
    printf("\nPressione Enter para concluir...");
    getchar();
#endif

    return 0;
}