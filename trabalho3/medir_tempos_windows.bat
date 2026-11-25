@echo off
cd /d "%~dp0"
set SAIDA=tempos_windows.txt
echo === Maquina === > %SAIDA%
powershell -NoProfile -Command "(Get-CimInstance Win32_Processor).Name; [math]::Round((Get-CimInstance Win32_ComputerSystem).TotalPhysicalMemory/1GB,1).ToString() + ' GB RAM'; (Get-CimInstance Win32_OperatingSystem).Caption" >> %SAIDA% 2>&1
echo === Compilador === >> %SAIDA%
gcc --version >> %SAIDA% 2>&1
echo === Python (para o plot) === >> %SAIDA%
python -c "import networkx, matplotlib, scipy; print('networkx', networkx.__version__, 'matplotlib', matplotlib.__version__)" >> %SAIDA% 2>&1
echo === Compilacao -O2 === >> %SAIDA%
gcc -O2 -Wall -Wextra joaomarcelo_francabatista_algoritmo.c -o trabalho3.exe >> %SAIDA% 2>&1
(echo A& echo G& echo 0) | trabalho3.exe joaomarcelo_francabatista_base.txt >> %SAIDA% 2>&1
echo === Compilacao -O0 === >> %SAIDA%
gcc -O0 -Wall -Wextra joaomarcelo_francabatista_algoritmo.c -o trabalho3_O0.exe >> %SAIDA% 2>&1
(echo A& echo G& echo 0) | trabalho3_O0.exe joaomarcelo_francabatista_base.txt >> %SAIDA% 2>&1
echo === FIM === >> %SAIDA%
