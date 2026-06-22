// kosmos-cli.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
    #define SEPARADOR "\\"
#else
    #include <unistd.h>
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
    char caminhoVscode[512], caminhoTasks[512], caminhoProps[512], cmdMkdir[512];
    
#ifdef _WIN32
    sprintf(caminhoVscode, "%s\\.vscode", nomeProjeto);
    sprintf(caminhoTasks, "%s\\.vscode\\tasks.json", nomeProjeto);
    sprintf(caminhoProps, "%s\\.vscode\\c_cpp_properties.json", nomeProjeto);
    sprintf(cmdMkdir, "mkdir \"%s\"", caminhoVscode);
#else
    sprintf(caminhoVscode, "%s/.vscode", nomeProjeto);
    sprintf(caminhoTasks, "%s/.vscode/tasks.json", nomeProjeto);
    sprintf(caminhoProps, "%s/.vscode/c_cpp_properties.json", nomeProjeto);
    sprintf(cmdMkdir, "mkdir -p \"%s\"", caminhoVscode);
#endif
    system(cmdMkdir);

    FILE* t = fopen(caminhoTasks, "w");
    if (t) {
#ifdef _WIN32
        fprintf(t, "{\n  \"version\": \"2.0.0\",\n  \"tasks\": [\n");
        fprintf(t, "    {\n      \"label\": \"KosmosUI: Build & Run (Windows)\",\n      \"type\": \"shell\",\n      \"command\": \"kosmos build\",\n");
        fprintf(t, "      \"group\": { \"kind\": \"build\", \"isDefault\": true },\n");
        fprintf(t, "      \"problemMatcher\": [\"$gcc\"],\n");
        fprintf(t, "      \"presentation\": { \"echo\": true, \"reveal\": \"always\", \"focus\": true, \"panel\": \"shared\", \"clear\": true }\n    }\n  ]\n}");
#else
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
        fprintf(t, "      \"dependsOn\": [\"4. Aplicar Manifest (Wine)\"], \"group\": \"build\", \"presentation\": { \"reveal\": \"silent\" }\n    },\n");
        fprintf(t, "    {\n      \"label\": \"Build All & Run (Linux)\", \"dependsOn\": [\"5. Gerar AppImage (Linux)\"], \"dependsOrder\": \"sequence\", \"group\": { \"kind\": \"build\", \"isDefault\": true },\n");
        fprintf(t, "      \"type\": \"process\", \"command\": \"${workspaceFolder}/output/linux/%s-x86_64.AppImage\", \"args\": [],\n", nomeProjeto);
        fprintf(t, "      \"presentation\": { \"echo\": false, \"reveal\": \"never\", \"focus\": false, \"panel\": \"shared\", \"showReuseMessage\": false, \"clear\": true }\n    }\n  ]\n}", nomeProjeto);
#endif
        fclose(t);
    }

    FILE* p = fopen(caminhoProps, "w");
    if (p) {
#ifdef _WIN32
        fprintf(p, "{\n  \"configurations\": [\n    {\n      \"name\": \"Win32-GCC\",\n");
        fprintf(p, "      \"includePath\": [\"${workspaceFolder}/**\", \"${workspaceFolder}/tools/kosmos/**\"],\n");
        fprintf(p, "      \"defines\": [\"_WIN32_WINNT=0x0A00\", \"WINVER=0x0A00\", \"UNICODE\", \"_UNICODE\", \"_WIN32\"],\n");
        fprintf(p, "      \"compilerPath\": \"gcc\", \"cStandard\": \"c17\", \"cppStandard\": \"gnu++17\", \"intelliSenseMode\": \"windows-gcc-x64\"\n    }\n  ],\n  \"version\": 4\n}");
#else
        fprintf(p, "{\n  \"configurations\": [\n    {\n      \"name\": \"Linux-MinGW-w64\",\n");
        fprintf(p, "      \"includePath\": [\"${workspaceFolder}/**\", \"/usr/x86_64-w64-mingw32/include\", \"/usr/share/mingw-w64/include\", \"${workspaceFolder}/tools/kosmos/**\"],\n");
        fprintf(p, "      \"defines\": [\"_WIN32_WINNT=0x0A00\", \"WINVER=0x0A00\", \"UNICODE\", \"_UNICODE\", \"__linux__\", \"_WIN32\"],\n");
        fprintf(p, "      \"compilerPath\": \"/usr/bin/x86_64-w64-mingw32-gcc\", \"cStandard\": \"c17\", \"cppStandard\": \"gnu++17\", \"intelliSenseMode\": \"linux-gcc-x64\",\n");
        fprintf(p, "      \"browse\": { \"path\": [\"/usr/x86_64-w64-mingw32/include\", \"${workspaceFolder}\"], \"limitSymbolsToIncludedHeaders\": true }\n    }\n  ],\n  \"version\": 4\n}");
#endif
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
    
#ifdef _WIN32
    if (strcmp(nome, ".") == 0) {
        sprintf(pathPrefixo, ".");
        sprintf(pastaResource, "resource");
        sprintf(pastaKosmos, "tools\\kosmos");
        sprintf(pastaMsys, "tools\\msys");
        sprintf(arquivoMain, "src\\main.c");
        sprintf(arquivoCore, "tools\\kosmos\\kosmos.c");
        sprintf(arquivoObjeto, "output\\resource.o");
        sprintf(arquivoSaida, "output\\%s.exe", nomeExecutavel);
    } else {
        sprintf(pathPrefixo, "%s", nome);
        sprintf(pastaResource, "%s\\resource", nome);
        sprintf(pastaKosmos, "%s\\tools\\kosmos", nome);
        sprintf(pastaMsys, "%s\\tools\\msys", nome);
        sprintf(arquivoMain, "%s\\src\\main.c", nome);
        sprintf(arquivoCore, "%s\\tools\\kosmos\\kosmos.c", nome);
        sprintf(arquivoObjeto, "%s\\output\\resource.o", nome);
        sprintf(arquivoSaida, "%s\\output\\%s.exe", nome, nomeExecutavel);
    }
#else
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
#endif

#ifdef _WIN32
    printf("🪟 [Windows Host] Iniciando compilação nativa local para Windows...\n");
    char cmdWin[2048];
    sprintf(cmdWin, "gcc -mwindows -I \"%s\" -I \"%s\" -O2 -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DUNICODE -D_UNICODE \"%s\" \"%s\" -o \"%s\" -lshcore -lcomctl32 -lgdi32 -luser32 -lshlwapi -lgdiplus -municode", pastaResource, pastaKosmos, arquivoMain, arquivoCore, arquivoSaida);
    int res = system(cmdWin);
    
    if (res == 0) {
        printf("✅ [Sucesso] Executável do Windows gerado em: %s\n", arquivoSaida);
        
        printf("\n[1/7] Limpando build anterior...\n");
        char cmdClean[512];
        sprintf(cmdClean, "if exist \"%s\\output\\linux\\%s.AppDir\" rmdir /S /Q \"%s\\output\\linux\\%s.AppDir\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdClean);
        
        char cmdMk[512];
        sprintf(cmdMk, "if not exist \"%s\\output\\linux\\%s.AppDir\\usr\\bin\" mkdir \"%s\\output\\linux\\%s.AppDir\\usr\\bin\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdMk);
        sprintf(cmdMk, "if not exist \"%s\\output\\linux\\%s.AppDir\\usr\\lib\" mkdir \"%s\\output\\linux\\%s.AppDir\\usr\\lib\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdMk);

        printf("[2/7] Copiando executável Windows do projeto (%s)...\n", arquivoSaida);
        char cmdCp[512];
        // 🌟 CORREÇÃO: Trocado de '&s' para '%s' para copiar o arquivo nativo de verdade
        sprintf(cmdCp, "copy /Y \"%s\" \"%s\\output\\linux\\%s.AppDir\\usr\\bin\\%s.exe\" > nul", arquivoSaida, pathPrefixo, nomeExecutavel, nomeExecutavel); system(cmdCp);

        char cmdCheckRes[512];
        sprintf(cmdCheckRes, "if exist \"%s\\resource\" (xcopy /E /I /Y \"%s\\resource\" \"%s\\output\\linux\\%s.AppDir\\usr\\bin\\resource\" > nul)", pathPrefixo, pathPrefixo, pathPrefixo, nomeExecutavel); system(cmdCheckRes);

        char pathIcon[512];
        sprintf(pathIcon, "%s\\resource\\icon.png", pathPrefixo);
        FILE *fIcon = fopen(pathIcon, "r");
        if (fIcon) {
            fclose(fIcon);
            sprintf(cmdCp, "copy /Y \"%s\\resource\\icon.png\" \"%s\\output\\linux\\%s.AppDir\\%s.png\" > nul", pathPrefixo, pathPrefixo, nomeExecutavel, nomeExecutavel); system(cmdCp);
        } else {
            char pathMockIcon[512];
            sprintf(pathMockIcon, "%s\\output\\linux\\%s.AppDir\\%s.png", pathPrefixo, nomeExecutavel, nomeExecutavel);
            FILE *fMock = fopen(pathMockIcon, "wb"); if (fMock) fclose(fMock);
        }

        sprintf(cmdMk, "if not exist \"%s\\tools\" mkdir \"%s\\tools\"", pathPrefixo, pathPrefixo); system(cmdMk);

        printf("[3/7] Verificando Wine-Staging 10.0 Portátil em tools/...\n");
        char pathWineCheck[512]; sprintf(pathWineCheck, "%s\\tools\\wine-portable.tar.xz", pathPrefixo);
        FILE *fWine = fopen(pathWineCheck, "r");
        if (!fWine) {
            printf("Baixando Wine 10.0 (Isso pode demorar um pouco)...\n");
            char cmdWineDl[1024];
            sprintf(cmdWineDl, "powershell -Command \"$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://github.com/Kron4ek/Wine-Builds/releases/download/10.0/wine-10.0-staging-amd64.tar.xz' -OutFile '%s\\tools\\wine-portable.tar.xz'\"", pathPrefixo);
            system(cmdWineDl);
        } else {
            fclose(fWine);
        }

        printf("Extraindo Wine para dentro do pacote...\n");
        char cmdExtract[1024];
        sprintf(cmdExtract, "tar -xf \"%s\\tools\\wine-portable.tar.xz\" --exclude=\"*/winecpp\" --exclude=\"*/wineg++\" -C \"%s\\output\\linux\\%s.AppDir\\usr\" --strip-components=1", pathPrefixo, pathPrefixo, nomeExecutavel);
        system(cmdExtract);

        printf("[4/7] Criando AppRun Dinâmico (Auto-DPI Baseado no Monitor do Usuário)...\n");
        char pathAppRun[512]; sprintf(pathAppRun, "%s\\output\\linux\\%s.AppDir\\AppRun", pathPrefixo, nomeExecutavel);
        FILE *fAppRun = fopen(pathAppRun, "wb"); 
        if (fAppRun) {
            fprintf(fAppRun, "#!/bin/bash\n\n");
            fprintf(fAppRun, "HERE=\"$(dirname \"$(readlink -f \"${0}\")\")\"\n");
            fprintf(fAppRun, "export PATH=\"$HERE/usr/bin:$PATH\"\n");
            fprintf(fAppRun, "export LD_LIBRARY_PATH=\"$HERE/usr/lib:$HERE/usr/lib64:$LD_LIBRARY_PATH\"\n");
            fprintf(fAppRun, "export WINEPREFIX=\"$HOME/.wine_kosmos_%s\"\n", nomeExecutavel);
            fprintf(fAppRun, "mkdir -p \"$WINEPREFIX\"\n\n");
            fprintf(fAppRun, "export WINEDEBUG=-all\nexport WINEARCH=win64\n\n");
            fprintf(fAppRun, "if [ ! -f \"$WINEPREFIX/configured\" ]; then\n");
            fprintf(fAppRun, "    echo \"Iniciando prefixo Wine...\"\n");
            fprintf(fAppRun, "    \"$HERE/usr/bin/wineboot\" -u\n");
            fprintf(fAppRun, "    touch \"$WINEPREFIX/configured\"\nfi\n\n");
            fprintf(fAppRun, "LINUX_SCALE=$(gsettings get org.gnome.desktop.interface scaling-factor 2>/dev/null | awk '{print $2}')\n\n");
            fprintf(fAppRun, "if [ -z \"$LINUX_SCALE\" ] || [ \"$LINUX_SCALE\" = \"0\" ]; then\n");
            fprintf(fAppRun, "    X_DPI=$(xrdb -query 2>/dev/null | grep dpi | awk '{print $2}' | cut -d. -f1)\n");
            fprintf(fAppRun, "    if [ -z \"$X_DPI\" ]; then X_DPI=130; fi\nelse\n    X_DPI=$((LINUX_SCALE * 192))\nfi\n\n");
            fprintf(fAppRun, "if [ \"$X_DPI\" -eq 96 ]; then\n");
            fprintf(fAppRun, "    TEXT_SCALE=$(gsettings get org.gnome.desktop.interface text-scaling-factor 2>/dev/null)\n");
            fprintf(fAppRun, "    if [ ! -z \"$TEXT_SCALE\" ] && [ \"$TEXT_SCALE\" != \"1.0\" ]; then\n");
            fprintf(fAppRun, "        if (( $(echo \"$TEXT_SCALE == 1.25\" | bc -l 2>/dev/null || echo 0) )); then X_DPI=120; fi\n");
            fprintf(fAppRun, "        if (( $(echo \"$TEXT_SCALE == 1.5\" | bc -l 2>/dev/null || echo 0) )); then X_DPI=144; fi\n");
            fprintf(fAppRun, "    fi\nfi\n\n");
            fprintf(fAppRun, "DPI_HEX=$(printf \"%%%%08x\" $X_DPI)\n");
            fprintf(fAppRun, "echo \"[KOSMOS] Monitor detectado: Aplicando $X_DPI DPI dinamicamente (Hex: $DPI_HEX)...\"\n\n");
            
            fprintf(fAppRun, "cat <<REG > \"$WINEPREFIX/config.reg\"\nREGEDIT4\n\n");
            fprintf(fAppRun, "[HKEY_CURRENT_USER\\Control Panel\\Desktop]\n\"LogPixels\"=dword:$DPI_HEX\n\"FontSmoothing\"=\"2\"\n\"FontSmoothingType\"=dword:00000002\n\"FontSmoothingOrientation\"=dword:00000001\n\"FontSmoothingGamma\"=dword:000004b0\n\"ForegroundLockTimeout\"=dword:00000000\n\n");
            fprintf(fAppRun, "[HKEY_CURRENT_USER\\Software\\Wine\\X11 Driver]\n\"Decorated\"=\"Y\"\n\"Managed\"=\"Y\"\n\"UseTakeFocus\"=\"N\"\n\n");
            fprintf(fAppRun, "[HKEY_CURRENT_USER\\Software\\Wine\\Fonts]\n\"Antialias\"=\"Y\"\n\n");
            fprintf(fAppRun, "[HKEY_CURRENT_USER\\Control Panel\\Colors]\n\"ActiveBorder\"=\"200 200 200\"\n\"ActiveTitle\"=\"255 255 255\"\n\"AppWorkspace\"=\"255 255 255\"\n\"Background\"=\"255 255 255\"\n\"ButtonAlternateFace\"=\"255 255 255\"\n\"ButtonDkShadow\"=\"160 160 160\"\n\"ButtonFace\"=\"243 243 243\"\n\"ButtonHilight\"=\"255 255 255\"\n\"ButtonLight\"=\"243 243 243\"\n\"ButtonShadow\"=\"200 200 200\"\n\"ButtonText\"=\"0 0 0\"\n\"GradientActiveTitle\"=\"255 255 255\"\n\"GradientInactiveTitle\"=\"243 243 243\"\n\"GrayText\"=\"120 120 120\"\n\"Hilight\"=\"0 120 215\"\n\"HilightText\"=\"255 255 255\"\n\"HotTrackingColor\"=\"0 102 204\"\n\"InactiveBorder\"=\"243 243 243\"\n\"InactiveTitle\"=\"243 243 243\"\n\"InactiveTitleText\"=\"120 120 120\"\n\"InfoText\"=\"0 0 0\"\n\"InfoWindow\"=\"255 255 255\"\n\"Menu\"=\"255 255 255\"\n\"MenuBar\"=\"243 243 243\"\n\"MenuHilight\"=\"230 230 230\"\n\"MenuText\"=\"0 0 0\"\n\"Scrollbar\"=\"243 243 243\"\n\"TitleText\"=\"0 0 0\"\n\"Window\"=\"255 255 255\"\n\"WindowFrame\"=\"200 200 200\"\n\"WindowText\"=\"0 0 0\"\n\n");
            fprintf(fAppRun, "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes]\n\"MS Shell Dlg\"=\"Segoe UI\"\n\"MS Shell Dlg 2\"=\"Segoe UI\"\n\"Tahoma\"=\"Segoe UI\"\n\"Arial\"=\"Segoe UI\"\nREG\n\n");
            fprintf(fAppRun, "\"$HERE/usr/bin/regedit\" /S \"$WINEPREFIX/config.reg\"\n");
            fprintf(fAppRun, "cd \"$HERE/usr/bin\"\n");
            fprintf(fAppRun, "exec \"$HERE/usr/bin/wine\" \"$HERE/usr/bin/%s.exe\" \"$@\"\n", nomeExecutavel);
            fclose(fAppRun);
        }

        printf("[5/7] Criar .desktop...\n");
        char pathDesktop[512]; sprintf(pathDesktop, "%s\\output\\linux\\%s.AppDir\\%s.desktop", pathPrefixo, nomeExecutavel, nomeExecutavel);
        FILE *fDesktop = fopen(pathDesktop, "wb");
        if (fDesktop) {
            fprintf(fDesktop, "[Desktop Entry]\nName=%s\nExec=AppRun\nIcon=%s\nType=Application\nCategories=Development;\n", nomeExecutavel, nomeExecutavel);
            fclose(fDesktop);
        }

        printf("[6/7] Verificando Runtime do AppImage em tools/...\n");
        char pathRuntimeCheck[512]; sprintf(pathRuntimeCheck, "%s\\tools\\runtime-x86_64", pathPrefixo);
        FILE *fRuntime = fopen(pathRuntimeCheck, "r");
        if (!fRuntime) {
            printf("Baixando Runtime...\n");
            char cmdRtDl[1024];
            sprintf(cmdRtDl, "powershell -Command \"$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://github.com/AppImage/AppImageKit/releases/download/continuous/runtime-x86_64' -OutFile '%s\\tools\\runtime-x86_64'\"", pathPrefixo);
            system(cmdRtDl);
        } else {
            fclose(fRuntime);
        }

        printf("[7/7] Empacotando AppImage...\n");
        char cmdMksquash[1024];
        sprintf(cmdMksquash, "\"\"%s\\mksquashfs.exe\" \"%s\\output\\linux\\%s.AppDir\" \"%s\\output\\linux\\fs.squashfs\" -root-owned -noappend -comp xz -b 1M\"\"", pastaMsys, pathPrefixo, nomeExecutavel, pathPrefixo);
        int rSquash = system(cmdMksquash);
        
        if (rSquash != 0) {
            printf("ERRO: mksquashfs não encontrado. Certifique-se de possuir o mksquashfs.exe em %s\n", pastaMsys);
            exit(1);
        }

        char cmdMerge[1024];
        sprintf(cmdMerge, "copy /b \"%s\\tools\\runtime-x86_64\" + \"%s\\output\\linux\\fs.squashfs\" \"%s\\output\\linux\\%s-x86_64.AppImage\" > nul", pathPrefixo, pathPrefixo, pathPrefixo, nomeExecutavel);
        system(cmdMerge);
        
        char cmdCleanUp[512];
        sprintf(cmdCleanUp, "del /Q \"%s\\output\\linux\\fs.squashfs\" && rmdir /S /Q \"%s\\output\\linux\\%s.AppDir\"", pathPrefixo, pathPrefixo, nomeExecutavel);
        system(cmdCleanUp);

        printf("======================================================\n");
        printf(" ✅ SUCESSO! AppImage COM AUTO-DPI DINÂMICO CRIADO:\n");
        printf(" 📍 Localização: %s\\output\\linux\\%s-x86_64.AppImage\n", pathPrefixo, nomeExecutavel);
        printf("======================================================\n");

        printf("\n🚀 [AUTO-RUN] Executando binário nativo do Windows local agora...\n");
        char cmdRunWin[512];
        // 🌟 CORREÇÃO DE DIRETÓRIO: Dá um 'cd' na pasta do projeto antes do app iniciar para achar o 'resource/' nativo
        if (strcmp(nome, ".") == 0) {
            sprintf(cmdRunWin, "\"%s\"", arquivoSaida);
        } else {
            sprintf(cmdRunWin, "cd \"%s\" && \"output\\%s.exe\"", pathPrefixo, nomeExecutavel);
        }
        system(cmdRunWin);
    } else {
        printf("❌ Erro durante o build local do Windows.\n");
    }
#else
    // --- FLUXO DO HOST LINUX ---
    printf("🔨 [1/4] Processando Recursos -> %s/resource.rc\n", pastaResource);
    char cmdWindres[1024];
    sprintf(cmdWindres, "/usr/bin/x86_64-w64-mingw32-windres -I \"%s\" -i \"%s/resource.rc\" -o \"%s\" -O coff -F pe-x86-64", pastaResource, pastaResource, arquivoObjeto);
    int r1 = system(cmdWindres);
    
    printf("🚀 [2/4] Executando Cross-Compiler GCC (MinGW-w64) -> %s\n", arquivoSaida);
    char cmdGcc[2048];
    sprintf(cmdGcc, "/usr/bin/x86_64-w64-mingw32-gcc -mwindows -g -Wall -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DUNICODE -D_UNICODE -Wl,--subsystem,windows -municode -I \"%s\" -I \"%s\" \"%s\" \"%s\" \"%s\" -o \"%s\" -lshcore -lcomctl32 -lgdi32 -luser32 -lshlwapi -lgdiplus -static-libgcc", pastaResource, pastaKosmos, arquivoMain, arquivoCore, arquivoObjeto, arquivoSaida);
    int r2 = system(cmdGcc);

    printf("🎭 [3/4] Injetando Manifest Visual do Windows via Wine...\n");
    char cmdManifest[1024];
    sprintf(cmdManifest, "WINEDEBUG=-all wine \"%s/mt.exe\" -nologo -manifest \"%s/kosmos.exe.manifest\" -outputresource:\"%s\";#1", pastaMsys, pastaKosmos, arquivoSaida);
    int r3 = system(cmdManifest);

    printf("📦 [4/4] Gerando Empacotamento Estável AppImage...\n");
    char cmdPackage[512];
    if (strcmp(nome, ".") == 0) {
        sprintf(cmdPackage, "bash tools/package_linux.sh \"%s\"", nomeExecutavel);
    } else {
        sprintf(cmdPackage, "cd \"%s\" && bash tools/package_linux.sh \"%s\"", nome, nomeExecutavel);
    }
    int r4 = system(cmdPackage);

    if (r1 == 0 && r2 == 0 && r3 == 0 && r4 == 0) {
        printf("✅ [Sucesso] Sequência de build executada com êxito! Projeto \"%s\" pronto.\n", nomeExecutavel);
        printf("🚀 [AUTO-RUN] Executando pacote AppImage isolado agora...\n");
        char cmdRunLinux[512];
        if (strcmp(nome, ".") == 0) {
            sprintf(cmdRunLinux, "./output/linux/%s-x86_64.AppImage", nomeExecutavel);
        } else {
            sprintf(cmdRunLinux, "./%s/output/linux/%s-x86_64.AppImage", nome, nomeExecutavel);
        }
        system(cmdRunLinux);
    } else {
        printf("❌ Erro em alguma etapa da compilação do projeto \"%s\".\n", nomeExecutavel);
    }
#endif
}

void implantarPacotes(const char* nome) {
    const char* nomeExecutavel = (strcmp(nome, ".") == 0) ? "projeto" : nome;
    char pathPrefixo[256];
    if (strcmp(nome, ".") == 0) {
        sprintf(pathPrefixo, ".");
    } else {
        sprintf(pathPrefixo, "%s", nome);
    }

#ifdef _WIN32
    printf("📦 [DEPLOY] Iniciando construção do instalador executável para Windows...\n");
    char pathIss[512];
    sprintf(pathIss, "%s\\output\\installer_config.iss", pathPrefixo);
    
    FILE *f = fopen(pathIss, "w");
    if (f) {
        fprintf(f, "[Setup]\nAppName=%s\nAppVersion=1.0.0\nDefaultDirName={pf}\\%s\nDefaultGroupName=%s\nOutputDir=..\\dist\nOutputBaseFilename=%s_Setup_v1.0.0\nCompression=lzma\nSolidCompression=yes\nDisableProgramGroupPage=yes\n\n[Files]\nSource: \"..\\output\\%s.exe\"; DestDir: \"{app}\"; Flags: ignoreversion\nSource: \"..\\resource\\*\"; DestDir: \"{app}\\resource\"; Flags: ignoreversion recursesubdirs createallsubdirs\n\n[Icons]\nName: \"{group}\\%s\"; Filename: \"{app}\\%s.exe\"\nName: \"{commondesktop}\\%s\"; Filename: \"{app}\\%s.exe\"; Tasks: desktopicon\n\n[Tasks]\nName: \"desktopicon\"; Description: \"Criar um atalho na Área de Trabalho\"; GroupDescription: \"Ícones Adicionais:\"\n\n[Run]\nFilename: \"{app}\\%s.exe\"; Description: \"Lançar %s agora\"; Flags: nowait postinstall skipifsilent\n", nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel);
        fclose(f);
    }

    char cmdDist[512];
    sprintf(cmdDist, "if not exist \"%s\\dist\" mkdir \"%s\\dist\"", pathPrefixo, pathPrefixo);
    system(cmdDist);

    printf("🔨 Compilando Setup com Inno Setup nativo...\n");
    char cmdCompile[1024];
    sprintf(cmdCompile, "\"C:\\Program Files (x86)\\Inno Setup 6\\ISCC.exe\" \"%s\"", pathIss);
    int res = system(cmdCompile);

    if (res != 0) {
        sprintf(cmdCompile, "iscc.exe \"%s\"", pathIss);
        res = system(cmdCompile);
    }

    char cmdClean[512];
    sprintf(cmdClean, "del /Q \"%s\"", pathIss);
    system(cmdClean);

    if (res == 0) {
        printf("=======================================================\n");
        printf("🥇 [GG] Distribuição Windows gerada com sucesso absoluto!\n");
        printf("📍 Localização: %s\\dist\\%s_Setup_v1.0.0.exe\n", pathPrefixo, nomeExecutavel);
        printf("=======================================================\n");
    } else {
        printf("❌ Erro: Falha ao compilar o instalador. Certifique-se de que o Inno Setup 6 está instalado no seu Windows.\n");
    }
#else
    printf("📦 [DEPLOY] Iniciando construção dos instaladores de distribuição pública...\n");
    char cmdDeb[512], cmdWin[512];
    sprintf(cmdDeb, "bash \"%s/tools/build_debian.sh\" \"%s\"", pathPrefixo, nomeExecutavel);
    sprintf(cmdWin, "bash \"%s/tools/build_windows_setup.sh\" \"%s\"", pathPrefixo, nomeExecutavel);
    
    int d1 = system(cmdDeb);
    int d2 = system(cmdWin);

    if (d1 == 0 && d2 == 0) {
        printf("🥇 [GG] Distribuição gerada! Arquivos .deb e .exe disponíveis dentro de %s/dist/\n", pathPrefixo);
    } else {
        printf("❌ Erro durante o empacotamento final dos instaladores.\n");
    }
#endif
}

// ================= FUNÇÃO PRINCIPAL (MAIN) =================

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    if (argc < 2) { mostrarAjuda(); return 1; }
    
    if (strcmp(argv[1], "create") == 0) {
        if (argc < 3) { printf("❌ Erro: Especifique o nome do projeto. Ex: kosmos create \"LotecaApp\"\n"); return 1; }
        criarProjeto(argv[2]);
    } 
    else if (strcmp(argv[1], "build") == 0) {
        const char* projetoAlvo = (argc < 3) ? "." : argv[2];
        compilarProjeto(projetoAlvo);
    }
    else if (strcmp(argv[1], "deploy") == 0) {
        const char* projetoAlvo = (argc < 3) ? "." : argv[2];
        implantarPacotes(projetoAlvo);
    }
    else if (strcmp(argv[1], "version") == 0) {
        printf("KosmosUI SDK Engine - v1.0.0 (Stable release 2026)\n");
    }
    else {
        mostrarAjuda();
    }
    return 0;
}
