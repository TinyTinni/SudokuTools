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
    Else
      SetGadgetText(gadgets(i), "")
    EndIf
  Next i
EndProcedure

Procedure.s grid_to_csv(Array values(1))
  result$ = ""
  For i.i = 1 To 81
    result$ = result$+Str(values(i-1))
    If ((i % 9) = 0)
      result$ = result$+~"\n";
    Else
      result$ = result$+","
    EndIf
  Next i
  ProcedureReturn result$
EndProcedure

Procedure csv_to_grid(text$, Array values(1))
  For i.i = 0 To 8
    line$ = StringField(text$, i+1, Chr(13))
    For k.i = 0 To 8
      s$ = StringField(line$, k+1, ",")
      If s$ <> ""
        s$ = Trim(s$)
        values(i*9+k) = Val(s$)
      Else
        values(i*9+k) = 0
      EndIf
    Next k
  Next i
  
EndProcedure

Global Dim string_gadgets.i(81)
Global Dim grid.i(81)

Procedure open_main_window()
  ;Create Window:
  OpenWindow(0, #PB_Ignore, #PB_Ignore, 500, 570, "Simple Sudoku Editor", #PB_Window_SystemMenu| #PB_Window_MinimizeGadget | #PB_Window_MaximizeGadget | #PB_Window_ScreenCentered)
  
  ;Add 2 menus:
  CreateMenu(0, WindowID(0))
  MenuTitle("&File")
  MenuItem(1, "&Load...")
  MenuItem(2, "&Save As...")
  MenuItem(3, "&Quit")
  ;FrameGadget
  If LoadFont(0, "Arial", 26)
    SetGadgetFont(#PB_Default, FontID(0))   ; Set the loaded Arial 16 font as new standard
  EndIf
  
  flags = #PB_String_Numeric;
  CompilerIf #PB_Compiler_OS = #PB_OS_Windows
    flags = flags | #PB_Text_Center;
  CompilerEndIf
  For i.i = 0 To 80
    string_gadgets(i) = StringGadget(#PB_Any, 15 + 52*(i%9), 15 + 52*(i/9), 50, 50,"", flags);
    SetGadgetAttribute(string_gadgets(i), #PB_String_MaximumLength, 1)
  Next i
  For i.i = 0 To 8
    FrameGadget(#PB_Any, (i/3)*156+13, (i%3)*156+13,158, 158, "", #PB_Frame_Flat)
  Next i
  
  SetGadgetFont(#PB_Default, #PB_Default)   ; Set the loaded Arial 16 font as new standard
  solve_button = ButtonGadget(#PB_Any, 170, 500, 157, 30, "Solve")
EndProcedure

open_main_window()

;Process window messages until closed:
Repeat
    Select WaitWindowEvent()
    Case #PB_Event_Menu
      Select EventMenu()
        Case 1:  ; Open
          File$ = OpenFileRequester("Please choose file to load", "", "CSV (*.csv)|*.csv", 0)
          If File$
            text$ = ""
            If ReadFile(0, File$)
              While Eof(0) = 0
                text$ = text$+ ReadString(0)+ Chr(13)
              Wend
              CloseFile(0)
              csv_to_grid(text$, grid())
              grid_to_gui(grid(), string_gadgets())
            EndIf
          EndIf
          ;Break
        Case 2: ; Save
          File$ = SaveFileRequester("Please choose file to save into", "", "CSV (*.csv)|*.csv", 0)
          If File$
            gui_to_grid(grid(), string_gadgets())
            csv$ = grid_to_csv(grid())
            f = CreateFile( #PB_Any, File$)
            WriteString(f, csv$)
            CloseFile(f)
          EndIf
         Case 3:
          Break
      EndSelect
    Case #PB_Event_Gadget
      Select EventGadget()
        Case solve_button : 
          gui_to_grid(grid(), string_gadgets())
          csv$ = grid_to_csv(grid())
          p = RunProgram("./solver", "", "", #PB_Program_Open | #PB_Program_Hide | #PB_Program_Write | #PB_Program_Read)
          If p
            WriteProgramString(p, csv$)
            WriteProgramData(p, #PB_Program_Eof , 1)
            While ProgramRunning(p)
              If AvailableProgramOutput(p)
                Output$ + ReadProgramString(p) + Chr(13)
              EndIf
            Wend
            csv_to_grid(Output$, grid())
            grid_to_gui(grid(), string_gadgets())
          Else
            MessageRequester("Could not start 'solver'", "Could not start 'solver'!", #PB_MessageRequester_Ok | #PB_MessageRequester_Error)
          EndIf
      EndSelect
    ;Case #PB_Event_SizeWindow: ResizeGadget(0, 0, 0, WindowWidth(0, #PB_Window_InnerCoordinate), WindowHeight(0, #PB_Window_InnerCoordinate))
    Case #PB_Event_CloseWindow: Break
    EndSelect
ForEver

; IDE Options = PureBasic 5.73 LTS (Windows - x64)
; CursorPosition = 80
; FirstLine = 65
; Folding = --
; EnableXP
; Executable = solver_gui