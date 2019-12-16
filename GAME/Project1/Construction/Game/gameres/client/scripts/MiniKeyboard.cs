//===========================================================================================================
// 文档说明:小键盘操作
// 创建时间:2009-3-2
// 创建人: soar
//===========================================================================================================

function MiniKeyboardDown0() //esc
{
	HideMiniKeyboard();
}

function MiniKeyboardDown1()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("~");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("`");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown2()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("!");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("1");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown3()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("@");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("2");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown4()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("#");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("3");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown5()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("$");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("4");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown6()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("%");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("5");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown7()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("^");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("6");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown8()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("&");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("7");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown9()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("*");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("8");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown10()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("(");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("9");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown11()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar(")");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("0");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown12()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("_");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("-");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown13()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("+");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("=");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown14()    //   Backspace
{
	AccountDeleteChar();
}

function MiniKeyboardDown15()    //   Tab
{
	AccountChangeInteracting();
}

function MiniKeyboardDown16()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("Q");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("q");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown17()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("W");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("w");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown18()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("E");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("e");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown19()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("R");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("r");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown20()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("T");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("t");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown21()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("Y");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("y");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown22()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("U");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("u");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown23()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("I");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("i");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown24()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("O");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("o");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown25()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("P");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("p");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown26()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("{");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("[");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown27()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("}");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("]");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown28()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("|");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("\\");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown30()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("A");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("a");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown31()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("S");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("s");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown32()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("D");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("d");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown33()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("F");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("f");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown34()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("G");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("g");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown35()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("H");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("h");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown36()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("J");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("j");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown37()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("K");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("k");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown38()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("L");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("l");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown39()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar(":");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar(";");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown40()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("\"");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("'");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown41()    //   Enter
{
	AccountHandleEntry();
}

function MiniKeyboardDown43()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("Z");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("z");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown44()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("X");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("x");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown45()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("C");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("c");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown46()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("V");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("v");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown47()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("B");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("b");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown48()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("N");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("n");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown49()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("M");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("m");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown50()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("<");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar(",");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown51()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar(">");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar(".");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown52()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar("?");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar("/");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function MiniKeyboardDown53()
{
	if((MiniKeyborad_42.IsStateOn() + MiniKeyborad_29.IsStateOn())%2 == 1)
	{
		AccountInsertChar(" ");
		MiniKeyborad_42.setStateOn(false);
	}
	else
	{
		AccountInsertChar(" ");
	}
	if(MiniKeyborad_42.IsStateOn() == 1)
		MiniKeyborad_42.setStateOn(false);
}

function AccountHandleEntry()
{
	fakeLogin();	
	HideMiniKeyboard();
}

function HideMiniKeyboard()
{
	MiniKeyboardGui.setVisible(false); 
}