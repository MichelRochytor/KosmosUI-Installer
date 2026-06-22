# 🌌 KosmosUI SDK Installer

Este repositório contém o ecossistema do instalador autónomo (*standalone*) e multiplataforma para o **KosmosUI SDK CLI (`kosmos`)**.

O grande diferencial deste instalador é ser **100% independente**: o usuário não precisa baixar arquivos `.zip` ou extrair pastas. Basta baixar o binário único diretamente da aba de **Releases** do GitHub e executar. O instalador faz o download dinâmico da versão estável da CLI, configura os compiladores necessários, injeta o barramento no `PATH` do sistema e deixa o ambiente pronto para o desenvolvimento.

---

## 🚀 Como Instalar (Para Usuários / GitHub Releases)

Escolha o executável ideal para o seu sistema operacional na aba de [Releases](https://github.com/MichelRochytor/KosmosUI-Installer/releases):

### 🪟 No Windows
1. Vá até as Releases e baixe apenas o arquivo **`instalar.exe`**.
2. Dê **dois cliques** no executável para iniciar o assistente gráfico.
3. Se o sistema não possuir o compilador GCC, o instalador invocará o *Windows Package Manager (Winget)* para configurar o MinGW-w64 silenciosamente.
4. **Pronto!** Feche o prompt, abra um novo terminal (CMD ou PowerShell) e digite `kosmos` para começar.

### 🐧 No Linux (Zorin OS / Ubuntu / Debian)
1. Vá até as Releases e baixe o arquivo **`instalar`** (binário nativo).
2. Dê permissão de execução ao arquivo. Você pode fazer isso graficamente (Propriedades > Permissões > Permitir executar como programa) ou via terminal:
   ```bash
   chmod +x instalar
