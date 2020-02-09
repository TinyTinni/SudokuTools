; Helping Prodecures
Procedure gui_to_grid(Array out.i(1), Array gadgets.i(1))
  For i.i = 0 To 80
    out(i) = Val(GetGadgetText(gadgets(i)))
  Next i
EndProcedure

Procedure grid_to_gui(Array in.i(1), Array gadgets.i(1))
  For i.i = 0 To 80
    If in(i) <> 0
      SetGadgetText(gadgets(i), Str(in(i)))
    EndIf
  Next i
EndProcedure

Procedure grid_to_csv(outputname.s, Array values(1))
  f = CreateFile( #PB_Any, outputname)
  For i.i = 1 To 81
    WriteString(f, Str(values(i-1)))
    If ((i % 9) = 0)
      WriteStringN(f, "")
    Else
      WriteString(f, ",")
    EndIf
  Next i
  CloseFile(f)
EndProcedure

Procedure csv_to_grid(filename$, Array values(1))
  If ReadFile(0, filename$)
    i.i = 0
    While Eof(0) = 0   
      line$ = ReadString(0)
      For k.i = 0 To 8
        s$ = StringField(line$, k+1, ",")
        If s$ <> ""
          values(i*9+k) = Val(s$)
        EndIf
      Next k
      i = i+1
    Wend
    CloseFile(0)
  EndIf
EndProcedure



Dim string_gadgets.i(81)
Dim grid.i(81)

;Create Window:
OpenWindow(0, #PB_Ignore, #PB_Ignore, 500, 570, "Simple Sudoku Editor", #PB_Window_SystemMenu| #PB_Window_MinimizeGadget | #PB_Window_MaximizeGadget | #PB_Window_SizeGadget)

;Add 2 menus:
CreateMenu(0, WindowID(0))
MenuItem(1, "&Load...")
MenuItem(2, "&Save As...")
;FrameGadget
If LoadFont(0, "Arial", 26)
  SetGadgetFont(#PB_Default, FontID(0))   ; Set the loaded Arial 16 font as new standard
EndIf

For i.i = 0 To 8
  FrameGadget(#PB_Any, (i/3)*156+14, (i%3)*156+14,157, 157, "", #PB_Frame_Flat)
Next i

For i.i = 0 To 80
  string_gadgets(i) = StringGadget(#PB_Any, 15 + 52*(i%9), 15 + 52*(i/9), 50, 50,"", #PB_String_Numeric|#PB_Text_Center)
  SetGadgetAttribute(string_gadgets(i), #PB_String_MaximumLength, 1)
Next i
SetGadgetFont(#PB_Default, #PB_Default)   ; Set the loaded Arial 16 font as new standard
solve_button = ButtonGadget(#PB_Any, 170, 500, 157, 25, "Solve")

;Process window messages until closed:
Repeat
    Select WaitWindowEvent()
    Case #PB_Event_Menu
      Select EventMenu()
        Case 1: 
          File$ = OpenFileRequester("Please choose file to load", "", "CSV (*.csv)|*.csv", 0)
          If File$
            csv_to_grid(File$, grid())
            grid_to_gui(grid(), string_gadgets())
          EndIf
          ;Break
        Case 2: 
          File$ = OpenFileRequester("Please choose file to save into", "", "CSV (*.csv)|*.csv", 0)
          If File$
            gui_to_grid(grid(), string_gadgets())
            grid_to_csv(File$, grid())
          EndIf
          ;Break
      EndSelect
    Case #PB_Event_Gadget
      Select EventGadget()
        Case solve_button : 
          ;gui_to_grid(grid(), string_gadgets())
          ;grid_to_csv("tmp_grid.csv", grid())
          ;p = RunProgram("sudoku_solver", "-i tmp_grid.csv -o tmp_solve.csv")
          ;While ProgramRunning(Compiler) Wend
          ;csv_to_grid("tmp_solve.csv", grid())
          ;grid_to_gui(grid(), string_gadgets())
      EndSelect
    ;Case #PB_Event_SizeWindow: ResizeGadget(0, 0, 0, WindowWidth(0, #PB_Window_InnerCoordinate), WindowHeight(0, #PB_Window_InnerCoordinate))
    Case #PB_Event_CloseWindow: Break
    EndSelect
ForEver
; IDE Options = PureBasic 5.71 LTS (Windows - x64)
; CursorPosition = 101
; FirstLine = 73
; Folding = -
; EnableXP
; DPIAware