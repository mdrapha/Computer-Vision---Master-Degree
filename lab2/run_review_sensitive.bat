@echo off
setlocal
set "ROOT=%~dp0"
set "PATH=C:\msys64\mingw64\bin;%PATH%"
"%ROOT%review_labels.exe" --images-dir "%ROOT%results_manual\sensitive\images" --csv "%ROOT%results_manual\sensitive\classification_template.csv"
endlocal
