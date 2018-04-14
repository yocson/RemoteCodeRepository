///////////////////////////////////////////////////////////////////////
// CodePopupWindow.xaml.cs - GUI for RemoteRepositoryPrototypes      //
// ver 1.0                                                           //
// Platform:    Macbook Pro 15, Windows 10 on Parallel Virtual Machine//
// Application: Project3, RemoteRepositoryPrototypes,                //
//				CSE687, Object Oriented Design, Spring 2018          //
// Author:      Cheng Wang,                                          //
//              cwang76@sry.edu                                      //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018 //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for RemoteRepositoryPrototypes.  
 * It's responsibilities are to:
 * - Popup window and show code file.
 *   
 * Required Files:
 * ---------------
 * CodePopupWindow.xaml, CodePopupWindow.xaml.cs
 * 
 * Build Process:
 * --------------
 * devenv RemoteRepositoryPrototypes.sln /rebuild debug
 * 
 * Maintenance History:
 * --------------------
 * ver 1.0 : 30 Apr 2018
 * - first release
 */


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace WpfApp1
{
    /// <summary>
    /// Interaction logic for CodePopupWindow.xaml
    /// </summary>
    public partial class CodePopupWindow : Window
    {
        public CodePopupWindow()
        {
            InitializeComponent();
        }
    }
}
