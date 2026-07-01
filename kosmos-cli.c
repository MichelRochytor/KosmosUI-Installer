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
    printf("================================================================================\n");
    printf("🌌 KOSMOSUI SDK CLI - Versão 1.1.0 (Build 2026)                                 \n");
    printf("================================================================================\n");
    printf("Uso: kosmos [comando] [argumentos]\n\n");
    printf("Comandos disponíveis:\n");
    printf("  create \"nome_projeto\"   Clona o repositório base e configura o .vscode\n");
    printf("  build [nome_projeto]    📦 Compila tudo, gera empacotamento completo (AppImage) e executa\n");
    printf("  run [nome_projeto]      ⚡ COMPILA RÁPIDO e executa o binário .exe direto (Sem gerar AppImage)\n");
    printf("  debug [nome_projeto]    🪲 Compila forçando o terminal ativo atrás da janela (Logs de printf)\n");
    printf("  deploy [nome_projeto]   Gera os pacotes de instalação finais (.deb e .exe)\n");
    printf("  version                 Exibe a versão atual do SDK\n");
    printf("================================================================================\n");
}

void criarAmbienteVscode(const char* nomeProjeto) {
    char caminhoVscode[1024], caminhoTasks[1024], caminhoProps[1024], cmdMkdir[2048];
    
// O #ifdef fica APENAS para os caminhos de pasta (por causa das barras \ e /)
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

    // 🌟 TASKS UNIVERSAL: O mesmo tasks.json roda "kosmos run" tanto no Windows quanto no Linux!
    FILE* t = fopen(caminhoTasks, "w");
    if (t) {
        fprintf(t, "{\n  \"version\": \"2.0.0\",\n  \"tasks\": [\n");
        fprintf(t, "    {\n      \"label\": \"KosmosUI: Fast Run\",\n      \"type\": \"shell\",\n      \"command\": \"kosmos run\",\n");
        fprintf(t, "      \"group\": { \"kind\": \"build\", \"isDefault\": true },\n");
        fprintf(t, "      \"problemMatcher\": [\"$gcc\"],\n");
        fprintf(t, "      \"presentation\": { \"echo\": true, \"reveal\": \"always\", \"focus\": true, \"panel\": \"shared\", \"clear\": true }\n    },\n");
        fprintf(t, "    {\n      \"label\": \"KosmosUI: Debug Mode\",\n      \"type\": \"shell\",\n      \"command\": \"kosmos debug\",\n");
        fprintf(t, "      \"problemMatcher\": [\"$gcc\"],\n");
        fprintf(t, "      \"presentation\": { \"echo\": true, \"reveal\": \"always\", \"focus\": true, \"panel\": \"shared\", \"clear\": true }\n    }\n  ]\n}");
        fclose(t);
    }

    // 🌟 PROPERTIES UNIVERSAL: JSON contém as duas configs e o VS Code se vira pra achar a certa!
    FILE* p = fopen(caminhoProps, "w");
    if (p) {
        fprintf(p, "{\n  \"configurations\": [\n");
        
        // --- BLOCO DO WINDOWS ---
        fprintf(p, "    {\n      \"name\": \"Win32\",\n");
        fprintf(p, "      \"includePath\": [\"${workspaceFolder}/**\", \"${workspaceFolder}/tools/kosmos/**\"],\n");
        fprintf(p, "      \"defines\": [\"_WIN32_WINNT=0x0A00\", \"WINVER=0x0A00\", \"UNICODE\", \"_UNICODE\", \"_WIN32\"],\n");
        fprintf(p, "      \"compilerPath\": \"gcc\", \"cStandard\": \"c17\", \"cppStandard\": \"gnu++17\", \"intelliSenseMode\": \"windows-gcc-x64\"\n    },\n");
        
        // --- BLOCO DO LINUX ---
        fprintf(p, "    {\n      \"name\": \"Linux\",\n");
        fprintf(p, "      \"includePath\": [\"${workspaceFolder}/**\", \"/usr/x86_64-w64-mingw32/include\", \"/usr/share/mingw-w64/include\", \"${workspaceFolder}/tools/kosmos/**\"],\n");
        fprintf(p, "      \"defines\": [\"_WIN32_WINNT=0x0A00\", \"WINVER=0x0A00\", \"UNICODE\", \"_UNICODE\", \"__linux__\", \"_WIN32\"],\n");
        fprintf(p, "      \"compilerPath\": \"/usr/bin/x86_64-w64-mingw32-gcc\", \"cStandard\": \"c17\", \"cppStandard\": \"gnu++17\", \"intelliSenseMode\": \"linux-gcc-x64\",\n");
        fprintf(p, "      \"browse\": { \"path\": [\"/usr/x86_64-w64-mingw32/include\", \"${workspaceFolder}\"], \"limitSymbolsToIncludedHeaders\": true }\n    }\n");
        
        fprintf(p, "  ],\n  \"version\": 4\n}");
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
    printf("👉 Execute: kosmos run \"%s\" para compilar e testar rapidamente.\n", nome);
}

// 🌟 ASSINATURA: Recebe `gerarAppImage` para saber se vai empacotar tudo no final
void compilarProjeto(const char* nome, int modoDebug, int gerarAppImage) {
    checarEInstalarCompilador();

    const char* nomeExecutavel = (strcmp(nome, ".") == 0) ? "projeto" : nome;
    char pathPrefixo[256];
    char pastaResource[1024], pastaKosmos[1024], pastaMsys[1024], arquivoMain[1024], arquivoCore[1024], arquivoObjeto[1024], arquivoSaida[1024];
    
#ifdef _WIN32
    if (strcmp(nome, ".") == 0) {
        sprintf(pathPrefixo, ".");
        sprintf(pastaResource, "resource");
        sprintf(pastaKosmos, "tools\\kosmos");
        sprintf(pastaMsys, "tools\\msys");
        // 🌟 WILDCARD: Pega qualquer .c dentro de src
        sprintf(arquivoMain, "\"src\"\\*.c"); 
        sprintf(arquivoCore, "tools\\kosmos\\kosmos.c");
        sprintf(arquivoObjeto, "output\\resource.o");
        sprintf(arquivoSaida, "output\\%s.exe", nomeExecutavel);
    } else {
        sprintf(pathPrefixo, "%s", nome);
        sprintf(pastaResource, "%s\\resource", nome);
        sprintf(pastaKosmos, "%s\\tools\\kosmos", nome);
        sprintf(pastaMsys, "%s\\tools\\msys", nome);
        // 🌟 WILDCARD: Pega qualquer .c dentro de src do projeto (espaços protegidos nas aspas iniciais)
        sprintf(arquivoMain, "\"%s\\src\"\\*.c", nome); 
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
        // 🌟 WILDCARD NO LINUX
        sprintf(arquivoMain, "\"src\"/*.c"); 
        sprintf(arquivoCore, "tools/kosmos/kosmos.c");
        sprintf(arquivoObjeto, "output/resource.o");
        sprintf(arquivoSaida, "output/%s.exe", nomeExecutavel);
    } else {
        sprintf(pathPrefixo, "%s", nome);
        sprintf(pastaResource, "%s/resource", nome);
        sprintf(pastaKosmos, "%s/tools/kosmos", nome);
        sprintf(pastaMsys, "%s/tools/msys", nome);
        // 🌟 WILDCARD NO LINUX
        sprintf(arquivoMain, "\"%s/src\"/*.c", nome); 
        sprintf(arquivoCore, "%s/tools/kosmos/kosmos.c", nome);
        sprintf(arquivoObjeto, "%s/output/resource.o", nome);
        sprintf(arquivoSaida, "%s/output/%s.exe", nome, nomeExecutavel);
    }
#endif

    printf("🪐 [Kosmos CLI] Preparando recursos e manifesto de forma nativa...\n");
    char rcOriginal[1024], rcTemporario[1024];
#ifdef _WIN32
    sprintf(rcOriginal, "%s\\resource.rc", pastaResource);
    sprintf(rcTemporario, "%s\\resource_temp.rc", pastaResource);
#else
    sprintf(rcOriginal, "%s/resource.rc", pastaResource);
    sprintf(rcTemporario, "%s/resource_temp.rc", pastaResource);
#endif

    char pastaKosmosLimpa[1024];
    strcpy(pastaKosmosLimpa, pastaKosmos);
    for (int i = 0; pastaKosmosLimpa[i] != '\0'; i++) {
        if (pastaKosmosLimpa[i] == '\\') {
            pastaKosmosLimpa[i] = '/'; // Normaliza para o windres não se perder com escapes
        }
    }

    FILE* fTemp = fopen(rcTemporario, "w");
    if (fTemp) {
        fprintf(fTemp, "1 24 \"%s/kosmos.exe.manifest\"\n\n", pastaKosmosLimpa);
        FILE* fOrig = fopen(rcOriginal, "r");
        if (fOrig) {
            char ch;
            while ((ch = fgetc(fOrig)) != EOF) {
                fputc(ch, fTemp);
            }
            fclose(fOrig);
        }
        fclose(fTemp);
    } else {
        printf("❌ [Erro] Não foi possível ler ou gerar arquivos de recurso temporários!\n");
        exit(1);
    }

#ifdef _WIN32
    // =========================================================================
    // --- FLUXO DO HOST WINDOWS ---
    // =========================================================================
    printf("🪟 [Windows Host] Iniciando compilação nativa...\n");
    
    char cmdRes[1024];
    sprintf(cmdRes, "windres -I \"%s\" -i \"%s\" -o \"%s\" -O coff -F pe-x86-64", pastaResource, rcTemporario, arquivoObjeto);
    int rRes = system(cmdRes);

    char cmdCleanTemp[1024];
    sprintf(cmdCleanTemp, "del \"%s\"", rcTemporario);
    system(cmdCleanTemp);

    if (rRes != 0) {
        printf("❌ Erro fatal: O windres falhou ao processar os recursos do sistema.\n");
        exit(1);
    }

    char cmdWin[4096];
    // Se modoDebug = 1, usa o subsistema 'console', senão remove o terminal usando o padrão 'windows'
    const char* subsistema = modoDebug ? "console" : "windows";
    
    // 🌟 ATENÇÃO AQUI: Retirei as aspas \" em volta do %s do arquivoMain para o wildcard (*.c) funcionar no CMD/Shell!
    // 🌟 ATENÇÃO AQUI 2: Adicionado -lm no final da string para a biblioteca math.h funcionar
    sprintf(cmdWin, "gcc -O2 -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DUNICODE -D_UNICODE -I \"%s\" -I \"%s\" %s \"%s\" \"%s\" -o \"%s\" -lcomctl32 -lshcore -lgdi32 -luser32 -lshlwapi -lgdiplus -static-libgcc -static-libstdc++ -Wl,--subsystem,%s -municode -lcomdlg32 -lm", 
            pastaResource, pastaKosmos, arquivoMain, arquivoCore, arquivoObjeto, arquivoSaida, subsistema);
    
    printf("⚙️  Vinculando binários da árvore de arquivos (*.c)...\n");
    int res = system(cmdWin);
    
    if (res == 0) {
        printf("✅ [Sucesso] Executável do Windows gerado em: %s\n", arquivoSaida);
        
        // 🌟 NOVA LÓGICA: Pula o empacotamento AppImage se for `kosmos run` ou `kosmos debug`
        if (modoDebug || !gerarAppImage) {
            if (modoDebug) {
                printf("\n🚀 [DEBUG-RUN] Inicializando aplicação anexada ao terminal atual...\n");
            } else {
                printf("\n🚀 [FAST-RUN] Inicializando aplicação nativa rapidamente...\n");
            }
            char cmdRunWin[1024];
            if (strcmp(nome, ".") == 0) {
                sprintf(cmdRunWin, "\"%s\"", arquivoSaida);
            } else {
                sprintf(cmdRunWin, "cd \"%s\" && \"output\\%s.exe\"", pathPrefixo, nomeExecutavel);
            }
            system(cmdRunWin);
            return; // 🛑 Para aqui e NÃO faz o AppImage!
        }

        // --- Fluxo Padrão de Geração de AppImage no Windows Host ---
        printf("\n[1/7] Limpando build anterior...\n");
        char cmdClean[1024];
        sprintf(cmdClean, "if exist \"%s\\output\\linux\\%s.AppDir\" rmdir /S /Q \"%s\\output\\linux\\%s.AppDir\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdClean);
        
        char cmdMk[1024];
        sprintf(cmdMk, "if not exist \"%s\\output\\linux\\%s.AppDir\\usr\\bin\" mkdir \"%s\\output\\linux\\%s.AppDir\\usr\\bin\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdMk);
        sprintf(cmdMk, "if not exist \"%s\\output\\linux\\%s.AppDir\\usr\\lib\" mkdir \"%s\\output\\linux\\%s.AppDir\\usr\\lib\"", pathPrefixo, nomeExecutavel, pathPrefixo, nomeExecutavel); system(cmdMk);

        printf("[2/7] Copiando executável Windows do projeto (%s)...\n", arquivoSaida);
        char cmdCp[1024];
        sprintf(cmdCp, "copy /Y \"%s\" \"%s\\output\\linux\\%s.AppDir\\usr\\bin\\%s.exe\" > nul", arquivoSaida, pathPrefixo, nomeExecutavel, nomeExecutavel); system(cmdCp);

        char cmdCheckRes[1024];
        sprintf(cmdCheckRes, "if exist \"%s\\resource\" (xcopy /E /I /Y \"%s\\resource\" \"%s\\output\\linux\\%s.AppDir\\usr\\bin\\resource\" > nul)", pathPrefixo, pathPrefixo, pathPrefixo, nomeExecutavel); system(cmdCheckRes);

        char pathIcon[1024];
        sprintf(pathIcon, "%s\\resource\\icon.png", pathPrefixo);
        FILE *fIcon = fopen(pathIcon, "r");
        if (fIcon) {
            fclose(fIcon);
            sprintf(cmdCp, "copy /Y \"%s\\resource\\icon.png\" \"%s\\output\\linux\\%s.AppDir\\%s.png\" > nul", pathPrefixo, pathPrefixo, nomeExecutavel, nomeExecutavel); system(cmdCp);
        } else {
            char pathMockIcon[1024];
            sprintf(pathMockIcon, "%s\\output\\linux\\%s.AppDir\\%s.png", pathPrefixo, nomeExecutavel, nomeExecutavel);
            FILE *fMock = fopen(pathMockIcon, "wb"); if (fMock) fclose(fMock);
        }

        sprintf(cmdMk, "if not exist \"%s\\tools\" mkdir \"%s\\tools\"", pathPrefixo, pathPrefixo); system(cmdMk);

        printf("[3/7] Verificando Wine-Staging 10.0 Portátil em tools/...\n");
        char pathWineCheck[1024]; sprintf(pathWineCheck, "%s\\tools\\wine-portable.tar.xz", pathPrefixo);
        FILE *fWine = fopen(pathWineCheck, "r");
        if (!fWine) {
            printf("Baixando Wine 10.0 (Isso pode demorar um pouco)...\n");
            char cmdWineDl[2048];
            sprintf(cmdWineDl, "powershell -Command \"$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://github.com/Kron4ek/Wine-Builds/releases/download/10.0/wine-10.0-staging-amd64.tar.xz' -OutFile '%s\\tools\\wine-portable.tar.xz'\"", pathPrefixo);
            system(cmdWineDl);
        } else {
            fclose(fWine);
        }

        printf("Extraindo Wine para dentro do pacote...\n");
        char cmdExtract[2048];
        sprintf(cmdExtract, "tar -xf \"%s\\tools\\wine-portable.tar.xz\" --exclude=\"*/winecpp\" --exclude=\"*/wineg++\" -C \"%s\\output\\linux\\%s.AppDir\\usr\" --strip-components=1", pathPrefixo, pathPrefixo, nomeExecutavel);
        system(cmdExtract);

        printf("[4/7] Criando AppRun Dinâmico (Auto-DPI Baseado no Monitor do Usuário)...\n");
        char pathAppRun[1024]; sprintf(pathAppRun, "%s\\output\\linux\\%s.AppDir\\AppRun", pathPrefixo, nomeExecutavel);
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
        char pathDesktop[1024]; sprintf(pathDesktop, "%s\\output\\linux\\%s.AppDir\\%s.desktop", pathPrefixo, nomeExecutavel, nomeExecutavel);
        FILE *fDesktop = fopen(pathDesktop, "wb");
        if (fDesktop) {
            fprintf(fDesktop, "[Desktop Entry]\nName=%s\nExec=AppRun\nIcon=%s\nType=Application\nCategories=Development;\n", nomeExecutavel, nomeExecutavel);
            fclose(fDesktop);
        }

        printf("[6/7] Verificando Runtime do AppImage em tools/...\n");
        char pathRuntimeCheck[1024]; sprintf(pathRuntimeCheck, "%s\\tools\\runtime-x86_64", pathPrefixo);
        FILE *fRuntime = fopen(pathRuntimeCheck, "r");
        if (!fRuntime) {
            printf("Baixando Runtime...\n");
            char cmdRtDl[2048];
            sprintf(cmdRtDl, "powershell -Command \"$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://github.com/AppImage/AppImageKit/releases/download/continuous/runtime-x86_64' -OutFile '%s\\tools\\runtime-x86_64'\"", pathPrefixo);
            system(cmdRtDl);
        } else {
            fclose(fRuntime);
        }

        printf("[7/7] Empacotando AppImage...\n");
        char cmdMksquash[2048];
        sprintf(cmdMksquash, "\"\"%s\\mksquashfs.exe\" \"%s\\output\\linux\\%s.AppDir\" \"%s\\output\\linux\\fs.squashfs\" -root-owned -noappend -comp xz -b 1M\"\"", pastaMsys, pathPrefixo, nomeExecutavel, pathPrefixo);
        int rSquash = system(cmdMksquash);
        
        if (rSquash != 0) {
            printf("ERRO: mksquashfs não encontrado. Certifique-se de possuir o mksquashfs.exe in %s\n", pastaMsys);
            exit(1);
        }

        char cmdMerge[2048];
        sprintf(cmdMerge, "copy /b \"%s\\tools\\runtime-x86_64\" + \"%s\\output\\linux\\fs.squashfs\" \"%s\\output\\linux\\%s-x86_64.AppImage\" > nul", pathPrefixo, pathPrefixo, pathPrefixo, nomeExecutavel);
        system(cmdMerge);
        
        char cmdCleanUp[1024];
        sprintf(cmdCleanUp, "del /Q \"%s\\output\\linux\\fs.squashfs\" && rmdir /S /Q \"%s\\output\\linux\\%s.AppDir\"", pathPrefixo, pathPrefixo, nomeExecutavel);
        system(cmdCleanUp);

        printf("======================================================\n");
        printf(" ✅ SUCESSO! AppImage COM AUTO-DPI DINÂMICO CRIADO:\n");
        printf(" 📍 Localização: %s\\output\\linux\\%s-x86_64.AppImage\n", pathPrefixo, nomeExecutavel);
        printf("======================================================\n");

        printf("\n🚀 [AUTO-RUN] Executando binário sem console por padrão...\n");
        char cmdRunWinEnd[1024];
        if (strcmp(nome, ".") == 0) {
            sprintf(cmdRunWinEnd, "\"%s\"", arquivoSaida);
        } else {
            sprintf(cmdRunWinEnd, "cd \"%s\" && \"output\\%s.exe\"", pathPrefixo, nomeExecutavel);
        }
        system(cmdRunWinEnd);
    } else {
        printf("❌ Erro durante o build local do Windows.\n");
    }
#else
    // =========================================================================
    // --- FLUXO DO HOST LINUX ---
    // =========================================================================
    FILE* fTempLinux = fopen(rcTemporario, "w");
    if (fTempLinux) {
        fprintf(fTempLinux, "1 24 \"%s/kosmos.exe.manifest\"\n\n", pastaKosmos);
        FILE* fOrigLinux = fopen(rcOriginal, "r");
        if (fOrigLinux) {
            char ch;
            while ((ch = fgetc(fOrigLinux)) != EOF) {
                fputc(ch, fTempLinux);
            }
            fclose(fOrigLinux);
        }
        fclose(fTempLinux);
    }

    printf("🔨 [1/3] Processando Recursos com Manifesto Acoplado -> %s\n", rcTemporario);
    char cmdWindres[2048];
    sprintf(cmdWindres, "/usr/bin/x86_64-w64-mingw32-windres -I \"%s\" -i \"%s\" -o \"%s\" -O coff -F pe-x86-64", pastaResource, rcTemporario, arquivoObjeto);
    int r1 = system(cmdWindres);
    
    char cmdCleanTemp[1024];
    sprintf(cmdCleanTemp, "rm \"%s\"", rcTemporario);
    system(cmdCleanTemp);
    
    printf("🚀 [2/3] Executando Cross-Compiler GCC (MinGW-w64) -> %s\n", arquivoSaida);
    char cmdGcc[4096];
    const char* subsistemaLinux = modoDebug ? "console" : "windows";

    // 🌟 ATENÇÃO AQUI: Retirei as aspas \" em volta do %s do arquivoMain para o wildcard (*.c) funcionar no Bash!
    // 🌟 ATENÇÃO AQUI 2: Adicionado -lm no final da string para a biblioteca math.h funcionar
    sprintf(cmdGcc, "/usr/bin/x86_64-w64-mingw32-gcc -mwindows -g -Wall -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DUNICODE -D_UNICODE -Wl,--subsystem,%s -municode -I \"%s\" -I \"%s\" %s \"%s\" \"%s\" -o \"%s\" -lcomctl32 -lshcore -lgdi32 -luser32 -lshlwapi -lgdiplus -static-libgcc -static-libstdc++ -lcomdlg32 -lm", 
            subsistemaLinux, pastaResource, pastaKosmos, arquivoMain, arquivoCore, arquivoObjeto, arquivoSaida);
    int r2 = system(cmdGcc);

    // 🌟 NOVA LÓGICA: Se for `run` ou `debug` no Linux, para por aqui e não monta SquashFS
    if (modoDebug || !gerarAppImage) {
        if (r1 == 0 && r2 == 0) {
            printf("✅ [Sucesso] Compilação %s efetuada (Árvore *.c montada).\n", modoDebug ? "de depuração" : "rápida");
            printf("🚀 [%s] Inicializando instância do Wine acoplada ao terminal atual...\n", modoDebug ? "DEBUG-RUN" : "FAST-RUN");
            char cmdRunLinux[1024];
            if (strcmp(nome, ".") == 0) {
                sprintf(cmdRunLinux, "wine %s", arquivoSaida);
            } else {
                sprintf(cmdRunLinux, "cd \"%s\" && wine output/%s.exe", nome, nomeExecutavel);
            }
            system(cmdRunLinux);
        } else {
            printf("❌ Erro ao compilar a árvore em modo %s.\n", modoDebug ? "debug" : "rápido");
        }
        return; // 🛑 Finaliza aqui para ignorar a montagem do AppImage!
    }

    printf("📦 [3/3] Gerando Empacotamento Estável AppImage...\n");
    char cmdPackage[1024];
    if (strcmp(nome, ".") == 0) {
        sprintf(cmdPackage, "bash tools/package_linux.sh \"%s\"", nomeExecutavel);
    } else {
        sprintf(cmdPackage, "cd \"%s\" && bash tools/package_linux.sh \"%s\"", nome, nomeExecutavel);
    }
    int r4 = system(cmdPackage);

    if (r1 == 0 && r2 == 0 && r4 == 0) {
        printf("✅ [Sucesso] Sequência de build executada com êxito! Projeto \"%s\" pronto.\n", nomeExecutavel);
        printf("🚀 [AUTO-RUN] Executando pacote AppImage isolado agora...\n");
        char cmdRunLinux[1024];
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
    char pathIss[1024];
    sprintf(pathIss, "%s\\output\\installer_config.iss", pathPrefixo);
    
    FILE *f = fopen(pathIss, "w");
    if (f) {
        fprintf(f, "[Setup]\nAppName=%s\nAppVersion=1.0.0\nDefaultDirName={pf}\\%s\nDefaultGroupName=%s\nOutputDir=..\\dist\nOutputBaseFilename=%s_Setup_v1.0.0\nCompression=lzma\nSolidCompression=yes\nDisableProgramGroupPage=yes\n\n[Files]\nSource: \"..\\output\\%s.exe\"; DestDir: \"{app}\"; Flags: ignoreversion\nSource: \"..\\resource\\*\"; DestDir: \"{app}\\resource\"; Flags: ignoreversion recursesubdirs createallsubdirs\n\n[Icons]\nName: \"{group}\\%s\"; Filename: \"{app}\\%s.exe\"\nName: \"{commondesktop}\\%s\"; Filename: \"{app}\\%s.exe\"; Tasks: desktopicon\n\n[Tasks]\nName: \"desktopicon\"; Description: \"Criar um atalho na Área de Trabalho\"; GroupDescription: \"Ícones Adicionais:\"\n\n[Run]\nFilename: \"{app}\\%s.exe\"; Description: \"Lançar %s agora\"; Flags: nowait postinstall skipifsilent\n", nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel, nomeExecutavel);
        fclose(f);
    }

    char cmdDist[1024];
    sprintf(cmdDist, "if not exist \"%s\\dist\" mkdir \"%s\\dist\"", pathPrefixo, pathPrefixo);
    system(cmdDist);

    printf("🔨 Compilando Setup com Inno Setup nativo...\n");
    char cmdCompile[2048];
    sprintf(cmdCompile, "\"C:\\Program Files (x86)\\Inno Setup 6\\ISCC.exe\" \"%s\"", pathIss);
    int res = system(cmdCompile);

    if (res != 0) {
        sprintf(cmdCompile, "iscc.exe \"%s\"", pathIss);
        res = system(cmdCompile);
    }

    char cmdClean[1024];
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
    char cmdDeb[1024], cmdWin[1024];
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
        // Modo build: MODO_DEBUG = 0 | GERAR_APPIMAGE = 1
        compilarProjeto(projetoAlvo, 0, 1); 
    }
    else if (strcmp(argv[1], "run") == 0) {
        const char* projetoAlvo = (argc < 3) ? "." : argv[2];
        // Modo run (Fast Run): MODO_DEBUG = 0 | GERAR_APPIMAGE = 0 (Pula o AppImage!)
        compilarProjeto(projetoAlvo, 0, 0); 
    }
    else if (strcmp(argv[1], "debug") == 0) {
        const char* projetoAlvo = (argc < 3) ? "." : argv[2];
        // Modo debug: MODO_DEBUG = 1 | GERAR_APPIMAGE = 0
        compilarProjeto(projetoAlvo, 1, 0); 
    }
    else if (strcmp(argv[1], "deploy") == 0) {
        const char* projetoAlvo = (argc < 3) ? "." : argv[2];
        implantarPacotes(projetoAlvo);
    }
    else if (strcmp(argv[1], "version") == 0) {
        printf("KosmosUI SDK Engine - v1.1.0 (Stable release 2026)\n");
    }
    else {
        mostrarAjuda();
    }
    return 0;
}