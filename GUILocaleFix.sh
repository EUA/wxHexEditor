for x in ./src/HexEditorGui.cpp ./src/HexEditorGui.h ./src/HexEditorCtrl/HexEditorCtrlGui.cpp ./src/HexEditorCtrl/HexEditorCtrlGui.h;
do
   sed -i 's/wxT(/_(/g' $x;
   sed -i "s/_('\\\t')/wxT('\\\t')/g" $x;
done;
                           
