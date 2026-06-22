// kosmos-cli.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #define SEPARADOR "\\\\"
#else
    #define SEPARADOR "/"
#endif

// ================= VALIDAÇÕES E COMPILADORES =================

void checarEInstalarGit() {
#ifdef _WIN32
    int temGit = (system("git --version > nul 2>&1") == 0);
    if (!temGit) {
        printf("⚠️  Git não encontrado no seu Windows!\n");
        printf("🔧 O KosmosUI vai instalar o Git para você via Windows Package Manager...\n");
        printf("Aguarde alguns instantes...\n");
        int res = system("winget install Git.Git --silent --accept-package-agreements --accept-source-agreements");
        if (res == 0) {
            printf("✅ Git instalado com sucesso! Por favor, REINICIE este prompt para o comando 'git' entrar em vigor.\n");
            exit(0);
        } else {
            printf("❌ Erro ao instalar o Git via winget. Por favor, instale o Git manualmente.\n");
            exit(1);
        }
    }
#else
    int temGit = (system("git --version > /dev/null 2>&1") == 0);
    if (!temGit) {
        printf("⚠️  Git não encontrado no seu Linux!\n");
        printf("🔧 O KosmosUI vai instalar o Git via APT (requer senha de sudo)...\n");
        int res = system("sudo apt update && sudo apt install -y git");
        if (res == 0) {
            printf("✅ Git instalado com sucesso no seu sistema! Prosseguindo...\n");
        } else {
            printf("❌ Falha ao instalar o Git via APT. Instale manualmente usando 'sudo apt install git'.\n");
            exit(1);
        }
    }
#endif
}

void checarEInstalarCompilador() {
#ifdef _WIN32
    int temGcc = (system("gcc --version > nul 2>&1") == 0);
    if (!temGcc) {
        printf("⚠️ GCC não encontrado no seu Windows!\n");
        printf("🔧 O KosmosUI vai instalar o MinGW-w64 (GCC) para você via Windows Package Manager...\n");
        printf("Aguarde alguns instantes...\n");
        int res = system("winget install BrechtSanders.MinGW --silent --accept-package-agreements --accept-source-agreements");
        if (res == 0) {
            printf("✅ GCC instalado com sucesso! Por favor, reinicie este prompt para atualizar o PATH do sistema.\n");
            exit(0);
        } else {
            printf("❌ Erro ao instalar via winget. Por favor, instale o MinGW manualmente.\n");
            exit(1);
        }
    }
#else
    int temMinGw = (system("x86_64-w64-mingw32-gcc --version > /dev/null 2>&1") == 0);
    if (!temMinGw) {
        printf("⚠️  Cross-Compiler MinGW-w64 não encontrado no seu Linux!\n");
        printf("🔧 O KosmosUI vai instalar as dependências de build necessárias via APT (requer senha de sudo)...\n");
        int res = system("sudo apt update && sudo apt install -y mingw-w64 binutils-mingw-w64 wine wine64");
        if (res == 0) {
            printf("✅ Todos os compiladores e o Wine foram instalados com sucesso! GG.\n");
        } else {
            printf("❌ Falha ao instalar dependências via APT. Execute 'sudo apt install mingw-w64' manualmente.\n");
            exit(1);
        }
    }
#endif
}

void mostrarAjuda() {
    printf("=======================================================\n");
    printf("🌌 KOSMOSUI SDK CLI - Versão 1.0.0 (Build 2026)       \n");
    printf("=======================================================\n");
    printf("Uso: kosmos [comando] [argumentos]\n\n");
    printf("Comandos disponíveis:\n");
    printf("  create \"nome_projeto\"   Clona o repositório base e configura o .vscode\n");
    printf("  build [nome_projeto]    Compila e executa automaticamente (Omitir nome assume pasta atual)\n");
    printf("  deploy [nome_projeto]   Gera os pacotes de instalação .deb e .exe (Omitir nome assume pasta atual)\n");
    printf("  version                  Exibe a versão atual do SDK\n");
    printf("=======================================================\n");
}

void criarAmbienteVscode(const char* nomeProjeto) {
    char caminhoVscode[512], caminhoTasks[512], caminhoProps[512];
    sprintf(caminhoVscode, "%s/.vscode", nomeProjeto);
    sprintf(caminhoTasks, "%s/.vscode/tasks.json", nomeProjeto);
    sprintf(caminhoProps, "%s/.vscode/c_cpp_properties.json", nomeProjeto);

#ifdef _WIN32
    char cmdMkdir[512]; sprintf(cmdMkdir, "mkdir \"%s\"", caminhoVscode); system(cmdMkdir);
#else
    char cmdMkdir[512]; sprintf(cmdMkdir, "mkdir -p \"%s\"", caminhoVscode); system(cmdMkdir);
#endif

    FILE* t = fopen(caminhoTasks, "w");
    if (t) {
        fprintf(t, "{\n  \"version\": \"2.0.0\",\n  \"tasks\": [\n");
        fprintf(t, "    {\n      \"label\": \"1. Compilar Recurso (Linux)\",\n      \"type\": \"shell\",\n      \"command\": \"/usr/bin/x86_64-w64-mingw32-windres\",\n");
        fprintf(t, "      \"args\": [\"-I\", \"${workspaceFolder}/resource\", \"-i\", \"${workspaceFolder}/resource/resource.rc\", \"-o\", \"${workspaceFolder}/output/resource.o\", \"-O\", \"coff\", \"-F\", \"pe-x86-64\"],\n");
        fprintf(t, "      \"group\": \"build\", \"problemMatcher\": [], \"presentation\": { \"reveal\": \"silent\" }\n    },\n");
        
        fprintf(t, "    {\n      \"label\": \"3. Compilar Aplicação Principal (Linux)\",\n      \"type\": \"shell\",\n      \"command\": \"/usr/bin/x86_64-w64-mingw32-gcc\",\n");
        fprintf(t, "      \"args\": [\"-mwindows\", \"-I\", \"${workspaceFolder}/resource\", \"-I\", \"${workspaceFolder}/tools/kosmos\", \"-g\", \"-Wall\", \"-D_WIN32_WINNT=0x0A00\", \"-DWINVER=0x0A00\", \"-DUNICODE\", \"-D_UNICODE\", \"${workspaceFolder}/src/main.c\", \"${workspaceFolder}/tools/kosmos/kosmos.c\", \"${workspaceFolder}/output/resource.o\", \"-o\", \"${workspaceFolder}/output/%s.exe\", \"-lshcore\", \"-lcomctl32\", \"-lgdi32\", \"-luser32\", \"-lshlwapi\", \"-lgdiplus\", \"-static-libgcc\", \"-Wl,--subsystem,windows\", \"-municode\"],\n", nomeProjeto);
        fprintf(t, "      \"dependsOn\": [\"1. Compilar Recurso (Linux)\"], \"group\": \"build\", \"problemMatcher\": [\"$gcc\"], \"presentation\": { \"reveal\": \"silent\" }\n    },\n");
        
        fprintf(t, "    {\n      \"label\": \"4. Aplicar Manifest (Wine)\",\n      \"type\": \"process\", \"command\": \"wine\",\n");
        fprintf(t, "      \"args\": [\"${workspaceFolder}/tools/msys/mt.exe\", \"-nologo\", \"-manifest\", \"Z:${workspaceFolder}/tools/kosmos/kosmos.exe.manifest\", \"-outputresource:Z:${workspaceFolder}/output/%s.exe;#1\"],\n", nomeProjeto);
        fprintf(t, "      \"options\": { \"env\": { \"WINEDEBUG\": \"-all\" } }, \"dependsOn\": \"3. Compilar Aplicação Principal (Linux)\", \"problemMatcher\": [], \"presentation\": { \"reveal\": \"silent\" }\n    },\n");
        
        fprintf(t, "    {\n      \"label\": \"5. Gerar AppImage (Linux)\",\n      \"type\": \"shell\", \"command\": \"bash\", \"args\": [\"${workspaceFolder}/tools/package_linux.sh\", \"%s\"],\n", nomeProjeto);
        fprintf(t, "      \"dependsOn\": [\"4. Aplicar Manifest (Wine)\", \"group\": \"build\", \"presentation\": { \"reveal\": \"silent\" }\n    },\n");
        
        fprintf(t, "    {\n      \"label\": \"Build All & Run (Linux)\", \"dependsOn\": [\"5. Gerar AppImage (Linux)\"], \"dependsOrder\": \"sequence\", \"group\": { \"kind\": \"build\", \"isDefault\": true },\n");
        fprintf(t, "      \"type\": \"process\", \"command\": \"${workspaceFolder}/output/linux/%s-x86_64.AppImage\", \"args\": [],\n", nomeProjeto);
        fprintf(t, "      \"presentation\": { \"echo\": false, \"reveal\": \"never\", \"focus\": false, \"panel\": \"shared\", \"showReuseMessage\": false, \"clear\": true }\n    }\n  ]\n}", nomeProjeto);
        fclose(t);
    }

    FILE* p = fopen(caminhoProps, "w");
    if (p) {
        fprintf(p, "{\n  \"configurations\": [\n    {\n      \"name\": \"Linux-MinGW-w64\",\n");
        fprintf(p, "      \"includePath\": [\"${workspaceFolder}/**\", \"/usr/x86_64-w64-mingw32/include\", \"/usr/share/mingw-w64/include\", \"${workspaceFolder}/tools/kosmos/**\"],\n");
        fprintf(p, "      \"defines\": [\"_WIN32_WINNT=0x0A00\", \"WINVER=0x0A00\", \"UNICODE\", \"_UNICODE\", \"__linux__\", \"_WIN32\"],\n");
        fprintf(p, "      \"compilerPath\": \"/usr/bin/x86_64-w64-mingw32-gcc\", \"cStandard\": \"c17\", \"cppStandard\": \"gnu++17\", \"intelliSenseMode\": \"linux-gcc-x64\",\n");
        fprintf(p, "      \"browse\": { \"path\": [\"/usr/x86_64-w64-mingw32/include\", \"${workspaceFolder}\"], \"limitSymbolsToIncludedHeaders\": true }\n    }\n  ],\n  \"version\": 4\n}");
        fclose(p);
    }
}

void criarProjeto(const char* nome) {
    checarEInstalarGit();

    printf("🌐 [KOSMOS] Conectando ao GitHub para clonar o repositório base do KosmosUI...\n");
    char cmdClone[1024];
    sprintf(cmdClone, "git clone https://github.com/MichelRochytor/KosmosUI.git \"%s\"", nome);
    int rClone = system(cmdClone);
    
    if (rClone != 0) {
        printf("❌ Erro: Falha ao executar o clone. Verifique o Git e a sua ligação à Internet.\n");
        return;
    }

    printf("🧹 Limpando metadados antigos do repositório Git...\n");
    char cmdResetGit[512];
#ifdef _WIN32
    sprintf(cmdResetGit, "rmdir /s /q \"%s\\\\.git\"", nome);
#else
    sprintf(cmdResetGit, "rm -rf \"%s/.git\"", nome);
#endif
    system(cmdResetGit);

    printf("⚙️  Gerando diretório de outputs e injetando configurações do VS Code...\n");
    char cmdOutput[512];
#ifdef _WIN32
    sprintf(cmdOutput, "mkdir \"%s\\\\output\"", nome); system(cmdOutput);
#else
    sprintf(cmdOutput, "mkdir -p \"%s/output\"", nome); system(cmdOutput);
#endif

    criarAmbienteVscode(nome);

    printf("\n🚀 [GG] Projeto \"%s\" inicializado com sucesso e pronto para desenvolvimento!\n", nome);
    printf("👉 Execute: kosmos build \"%s\" para compilar e rodar a árvore completa.\n", nome);
}

void compilarProjeto(const char* nome) {
    checarEInstalarCompilador();

    const char* nomeExecutavel = (strcmp(nome, ".") == 0) ? "projeto" : nome;
    char pathPrefixo[256];
    char pastaResource[512], pastaKosmos[512], pastaMsys[512], arquivoMain[512], arquivoCore[512], arquivoObjeto[512], arquivoSaida[512];
    
    if (strcmp(nome, ".") == 0) {
        sprintf(pathPrefixo, ".");
        sprintf(pastaResource, "resource");
        sprintf(pastaKosmos, "tools/kosmos");
        sprintf(pastaMsys, "tools/msys");
        sprintf(arquivoMain, "src/main.c");
        sprintf(arquivoCore, "tools/kosmos/kosmos.c");
        sprintf(arquivoObjeto, "output/resource.o");
        sprintf(arquivoSaida, "output/%s.exe", nomeExecutavel);
    } else {
        sprintf(pathPrefixo, "%s", nome);
        sprintf(pastaResource, "%s/resource", nome);
        sprintf(pastaKosmos, "%s/tools/kosmos", nome);
        sprintf(pastaMsys, "%s/tools/msys", nome);
        sprintf(arquivoMain, "%s/src/main.c", nome);
        sprintf(arquivoCore, "%s/tools/kosmos/kosmos.c", nome);
        sprintf(arquivoObjeto, "%s/output/resource.o", nome);
        sprintf(arquivoSaida, "%s/output/%s.exe", nome, nomeExecutavel);
    }

#ifdef _WIN32
    printf("🪟 [Windows Host] Iniciando compilação nativa local para Windows...\n");
    char cmdWin[2048];
    sprintf(cmdWin, "gcc -mwindows -I \"%%s\" -I \"%%s\" -O2 -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DUNICODE -D_UNICODE \"%%s\" \"%%s\" -o \"%%s\" -lshcore -lcomctl32 -lgdi32 -luser32 -lshlwapi -lgdiplus -municode", pastaResource, pastaKosmos, arquivoMain, arquivoCore, arquivoSaida);
    int res = system(cmdWin);
    
    if (res == 0) {
        printf("Hex: ✅ [Sucesso] Executável do Windows gerado em: %%s\n", arquivoSaida);
        
        printf("\n[1/7] Limpando build anterior...\n");
        char cmdClean[512];
        sprintf(cmdClean, "if exist \"%%s\\output\\linux\\%%s.AppDir\" rmdir /S /Q \"%%s\\output\\linux\\%%s.AppDir\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdClean);
        
        char cmdMk[512];
        sprintf(cmdMk, "if not exist \"%%s\\output\\linux\\%%s.AppDir\\usr\\bin\" mkdir \"%%s\\output\\linux\\%%s.AppDir\\usr\\bin\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdMk);
        sprintf(cmdMk, "if not exist \"%%s\\output\\linux\\%%s.AppDir\\usr\\lib\" mkdir \"%%s\\output\\linux\\%%s.AppDir\\usr\\lib\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdMk);

        printf("[2/7] Copiando executável Windows do projeto (%%s)...\n", arquivoSaida);
        char cmdCp[512];
        sprintf(cmdCp, "copy /Y \"%%s\" \"%%s\\output\\linux\\%%s.AppDir\\usr\\bin\\%%s.exe\" > nul", arquivoSaida, pathPrefixo, nomeExecutavel, nomeExecutavel); system(cmdCp);

        if (system("if exist \"resource\" (exit 0) else (exit 1)") == 0) {
            sprintf(cmdCp, "xcopy /E /I /Y \"%%s\\resource\" \"%%s\\output\\linux\\%%s.AppDir\\usr\\bin\\resource\" > nul", pathPrefixo, pathPrefixo, nomeExecutavel); system(cmdCp);
        }

        char pathIcon[512];
        sprintf(pathIcon, "%%s\\resource\\icon.png", pathPrefixo);
        FILE *fIcon = fopen(pathIcon, "r");
        if (fIcon) {
            fclose(fIcon);
            sprintf(cmdCp, "copy /Y \"%%s\\resource\\icon.png\" \"%%s\\output\\linux\\%%s.AppDir\\%%s.png\" > nul", pathPrefixo, pathPrefixo, nomeExecutavel, nomeExecutavel); system(cmdCp);
        } else {
            char cmdWgetIcon[1024];
            sprintf(cmdWgetIcon, "powershell -Command \"$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://upload.wikimedia.org/wikipedia/commons/8/83/Circle-icons-dev.svg' -OutFile '%%s\\output\\linux\\%%s.AppDir\\%%s.svg'\"", pathPrefixo, nomeExecutavel, nomeExecutavel);
            system(cmdWgetIcon);
        }

        sprintf(cmdMk, "if not exist \"%%s\\tools\" mkdir \"%%s\\tools\"", pathPrefixo, pathPrefixo); system(cmdMk);

        printf("[3/7] Verificando Wine-Staging 10.0 Portátil em tools/...\n");
        char pathWineCheck[512]; sprintf(pathWineCheck, "%%s\\tools\\wine-portable.tar.xz", pathPrefixo);
        FILE *fWine = fopen(pathWineCheck, "r");
        if (!fWine) {
            printf("Baixando Wine 10.0 (Isso pode demorar um pouco)...\n");
            char cmdWineDl[1024];
            sprintf(cmdWineDl, "powershell -Command \"$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://github.com/Kron4ek/Wine-Builds/releases/download/10.0/wine-10.0-staging-amd64.tar.xz' -OutFile '%%s\\tools\\wine-portable.tar.xz'\"", pathPrefixo);
            system(cmdWineDl);
        } else {
            fclose(fWine);
        }

        printf("Extraindo Wine para dentro do pacote...\n");
        char cmdExtract[1024];
        sprintf(cmdExtract, "tar -xf \"%%s\\tools\\wine-portable.tar.xz\" -C \"%%s\\output\\linux\\%%s.AppDir\\usr\" --strip-components=1", pathPrefixo, pathPrefixo, nomeExecutavel);
        system(cmdExtract);

        printf("[4/7] Criando AppRun Dinâmico (Auto-DPI Baseado no Monitor do Usuário)...\n");
        char pathAppRun[512]; sprintf(pathAppRun, "%%s\\output\\linux\\%%s.AppDir\\AppRun", pathPrefixo, nomeExecutavel);
        FILE *fAppRun = fopen(pathAppRun, "wb"); 
        if (fAppRun) {
            fprintf(fAppRun, "#!/bin/bash\n\n");
            fprintf(fAppRun, "HERE=\"$(dirname \"$(readlink -f \"${0}\")\")\"\n");
            fprintf(fAppRun, "export PATH=\"$HERE/usr/bin:$PATH\"\n");
            fprintf(fAppRun, "export LD_LIBRARY_PATH=\"$HERE/usr/lib:$HERE/usr/lib64:$LD_LIBRARY_PATH\"\n");
            fprintf(fAppRun, "export WINEPREFIX=\"$HOME/.wine_kosmos_%%s\"\n", nomeExecutavel);
            fprintf(fAppRun, "mkdir -p \"$WINEPREFIX\"\n\n");
            fprintf(fAppRun, "export WINEDEBUG=-all\nexport WINEARCH=win64\n\n");
            fprintf(fAppRun, "if [ ! -f \"$WINEPREFIX/configured\" ]; then\n");
            fprintf(fAppRun, "    echo \"Iniciando prefixo Wine...\"\n");
            fprintf(fAppRun, "    \"$HERE/usr/bin/wineboot\" -u\n");
            fprintf(fAppRun, "    touch \"$WINEPREFIX/configured\"\nfi\n\n");
            fprintf(fAppRun, "LINUX_SCALE=$(gsettings get org.gnome.desktop.interface scaling-factor 2>/dev/null | awk '{print $2}')\n\n");
            fprintf(fAppRun
