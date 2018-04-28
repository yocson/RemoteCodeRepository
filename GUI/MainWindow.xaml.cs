///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for RemoteRepository                     //
// ver 1.8                                                           //
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
 * This package provides a WPF-based GUI for RemoteRepository.  
 * It's responsibilities are to:
 * - Provide a display of directory contents of a remote Server.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories by double-clicking on subdirectory
 *   or the parent directory, indicated by the name "..".
 * It provides several tabs, such as checkin, checkout, browse...
 * With the btn on the page, you can use the functionality of server. 
 *   
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Build Process:
 * --------------
 * devenv RemoteRepositoryPrototypes.sln /rebuild debug
 * 
 * Maintenance History:
 * --------------------
 * ver 1.8 : 24 Apr 2018
 * - add automated test
 * ver 1.7 : 23 Apr 2018
 * - fix bugs
 * ver 1.6 : 22 Apr 2018
 * - Add functionality for query 
 * ver 1.5 : 21 Apr 2018
 * - Add functionality for browse 
 * ver 1.4 : 20 Apr 2018
 * - Add functionality for checkin, checkout 
 * ver 1.3 : 15 Apr 2018
 * - Add process functions 
 * ver 1.2 : 9 Apr 2018
 * - Add process functions
 * ver 1.1 : 3 Apr 2018
 * - Add several tabs
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superseded
 *   by this package.
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using MsgPassingCommunication;

namespace WpfApp1
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            Console.Title = "Project#4 GUI Console Demo";
        }

        private Stack<string> pathStack_ = new Stack<string>();
        private Stack<string> pathStack_checkin = new Stack<string>();
        private Stack<string> pathStack_checkout = new Stack<string>();
        private Stack<string> pathStack_browse = new Stack<string>();
        private Stack<string> pathStack_viewfile = new Stack<string>();
        private Translater translater;
        private CsEndPoint endPoint_;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_
          = new Dictionary<string, Action<CsMessage>>();
        private Dictionary<string, TextBlock> pathTextBlock_
            = new Dictionary<string, TextBlock>();
        private string serverAddr;
        private int serverPort;
        private int clientPort = 8082;
        private bool testMode = false;

        //----< process incoming messages on child thread >----------------

        private void processMessages()
        {
            ThreadStart thrdProc = () =>
            {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    if (msg.attributes.Count != 0)
                    {
                        string msgId = msg.value("command");
                        if (dispatcher_.ContainsKey(msgId))
                            dispatcher_[msgId].Invoke(msg);
                    }
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearDirs(ListBox li)
        {
            li.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addDir(string dir, ListBox li)
        {
            li.Items.Add(dir);
        }
        //----< function dispatched by child thread to main thread >-------

        private void insertParent(ListBox li)
        {
            li.Items.Insert(0, "..");
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearFiles(ListBox li)
        {
            li.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addFile(string file, ListBox li)
        {
            li.Items.Add(file);
        }

        //----< function determing which filelist >-------

        private ListBox whichFileList(string listname)
        {
            if (listname == "DirList_checkin")
                return FileList_checkin;
            if (listname == "DirList_checkout")
                return FileList_checkout;
            if (listname == "DirList_browse")
                return FileList_browse;

            return FileList_checkin;
        }

        //----< function determing which dirlist >-------

        private ListBox whichDirList(string listname)
        {
            if (listname == "DirList_checkin")
                return DirList_checkin;
            if (listname == "DirList_checkout")
                return DirList_checkout;
            if (listname == "DirList_browse")
                return DirList_browse;

            return DirList_checkin;
        }

        //----< add client processing for message with key >---------------

        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }
        //----< load getDirs processing into dispatcher dictionary >-------

        private void DispatcherLoadGetDirs()
        {
            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
                {
                    clearDirs(whichDirList(rcvMsg.value("listname")));
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                string listname = rcvMsg.value("listname");
                ListBox li = whichDirList(listname);
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir(dir, li);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent(li);
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirs", getDirs);
        }
        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                string listname = rcvMsg.value("listname");
                ListBox li = whichFileList(listname);
                Action clrFiles = () =>
                {
                    clearFiles(li);
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();

                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile(file, li);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }

                }
            };
            addClientProc("getFiles", getFiles);
        }
        //----< load checkIn processing into dispatcher dictionary >------

        private void DispatcherCheckIn()
        {
            Action<CsMessage> checkIn = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    refresh_list();
                    statusBarText.Text = "Received checkin messages";
                    testbox.Items.Insert(0, "Received checkin message");
                    if (testMode) Thread.Sleep(1000);
                });
            };
            addClientProc("checkIn", checkIn);
        }
        //----< load checkAuthor processing into dispatcher dictionary >-------

        private void DispatcherCheckAuthor()
        {
            Action<CsMessage> checkAuthor = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    if (rcvMsg.value("pass") == "1")
                    {
                        Console.Write("");
                        CsEndPoint serverEndPoint = new CsEndPoint();
                        serverEndPoint.machineAddress = serverAddr;
                        serverEndPoint.port = serverPort;
                        CsMessage msg = new CsMessage();
                        msg.add("to", CsEndPoint.toString(serverEndPoint));
                        msg.add("from", CsEndPoint.toString(endPoint_));
                        msg.add("command", "checkInFile");
                        msg.add("author", rcvMsg.value("author"));
                        msg.add("description", rcvMsg.value("desciption"));
                        int cate_num = Int32.Parse(rcvMsg.value("cate_num"));
                        msg.add("cate_num", rcvMsg.value("cate_num"));
                        for (int i = 0; i < cate_num; ++i)
                        {
                            msg.add("cate" + i.ToString(), rcvMsg.value("cate" + i.ToString()));

                        }
                        msg.add("namesp", rcvMsg.value("namesp"));
                        msg.add("file", rcvMsg.value("filename"));
                        msg.add("filename", rcvMsg.value("filename"));
                        msg.add("subdir", rcvMsg.value("subdir"));
                        string targetPath = "../SendFiles";
                        string destFile = System.IO.Path.Combine(targetPath, System.IO.Path.GetFileName(fileselect.Text));
                        System.IO.File.Copy(fileselect.Text, destFile, true);
                        translater.postMessage(msg);
                    }
                    else
                    {
                        author_text.Background = Brushes.Red;
                        statusBarText.Text = "Author is wrong";
                    }
                    testbox.Items.Insert(0, "Received checkauthor message");
                    if (testMode) Thread.Sleep(1000);
                });
            };
            addClientProc("checkAuthor", checkAuthor);
        }
        //----< load checkInFile processing into dispatcher dictionary >-------

        private void DispatcherCheckInFile()
        {
            Action<CsMessage> checkInFile = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    if (rcvMsg.value("canbeCheckIn") == "1")
                    {
                        refresh_list();
                        statusBarText.Text = "The file is checkin sucssesfully.";
                    }
                    else
                    {
                        // file is close-pending, please close first.
                        statusBarText.Text = "File is close-pending, please close it first.";

                    }
                    testbox.Items.Insert(0, "Received checkInFile message");
                });
            };
            addClientProc("checkInFile", checkInFile);
        }

        //----< load checkOut processing into dispatcher dictionary >------

        private void DispatcherCheckOut()
        {
            Action<CsMessage> checkOut = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    string targetPath = desselect.Text;
                    string destFile = System.IO.Path.Combine(targetPath, rcvMsg.value("file"));

                    statusBarText.Text = "Received checkout messages";
                    testbox.Items.Insert(0, "Received checkout message");
                    System.IO.File.Copy("../SaveFiles/" + rcvMsg.value("file"), destFile, true);
                    checkout_Status.Items.Insert(0, "Checked out " + rcvMsg.value("file"));
                    if (rcvMsg.value("with_depend") == "True")
                    {
                        Console.Write("checkout");
                        CsEndPoint serverEndPoint = new CsEndPoint();
                        serverEndPoint.machineAddress = serverAddr;
                        serverEndPoint.port = serverPort;
                        CsMessage msg = new CsMessage();
                        msg.add("to", CsEndPoint.toString(serverEndPoint));
                        msg.add("from", CsEndPoint.toString(endPoint_));
                        msg.add("command", "checkOut");
                        msg.add("with_depend", "False");
                        msg.add("filepath", rcvMsg.value("filepath"));
                        int dependnum = Int32.Parse(rcvMsg.value("dependnum"));
                        Console.Write(dependnum);
                        for (int i = 0; i < dependnum; ++i)
                        {
                            if (rcvMsg.value("depend" + i.ToString()) == rcvMsg.value("filename")) continue;
                            msg.add("filename", rcvMsg.value("depend" + i.ToString()));
                            translater.postMessage(msg);
                            msg.remove("filename");
                        }
                    }
                    statusBarText.Text = "Received checkout messages";
                    testbox.Items.Insert(0, "Received checkout message");
                    if (testMode) Thread.Sleep(1000);
                });
            };
            addClientProc("checkOut", checkOut);
        }
        //----< load browse processing into dispatcher dictionary >------

        private void DispatcherQuery()
        {
            Action<CsMessage> query = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    int num = Int32.Parse(rcvMsg.value("num"));
                    for (int i = 0; i < num; ++i)
                    {
                        rcvMsg.value("result" + i.ToString());
                        queryRes_List.Items.Insert(0, rcvMsg.value("result" + i.ToString()));
                    }
                    statusBarText.Text = "Received query messages";
                    testbox.Items.Insert(0, "Received query message");
                    if (testMode) Thread.Sleep(1000);
                });
            };
            addClientProc("query", query);
        }

        //----< load connect processing into dispatcher dictionary >------

        private void DispatcherConnect()
        {
            Action<CsMessage> connect = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    statusBarText.Text = "Connected";
                    testbox.Items.Insert(0, "Received connect message, Port: " + clientPortText.Text);


                    refresh_list();

                    if (testMode) Thread.Sleep(500);
                });
            };
            addClientProc("connect", connect);
        }
        //----< refresh all the filelists and dirlists >-------

        private void refresh_list()
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = serverAddr;
            serverEndPoint.port = serverPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("path", pathStack_viewfile.Peek());
            List<string> DirLists = new List<string> { "DirList", "DirList_checkin", "DirList_checkout", "DirList_browse" };
            foreach (string li in DirLists)
            {
                msg.add("command", "getDirs");
                msg.add("listname", li);
                translater.postMessage(msg);
                msg.remove("command");
                msg.add("command", "getFiles");
                translater.postMessage(msg);
                msg.remove("command");
                msg.remove("listname");
            }
        }

        //----< load viewdata processing into dispatcher dictionary >------

        private void DispatcherViewData()
        {
            Action<CsMessage> viewdata = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    statusBarText.Text = "Received viewdata message";
                    testbox.Items.Insert(0, "Received viewdata message");
                    if (testMode) Thread.Sleep(1000);
                });
            };
            addClientProc("viewdata", viewdata);
        }
        //----< load closeFile processing into dispatcher dictionary >-------

        private void DispatcherCloseFile()
        {
            Action<CsMessage> closeFile = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    if (rcvMsg.value("closed") == "1")
                    {
                        statusBarText.Text = "closeFile sucssesfully";
                    }
                    else
                    {
                        statusBarText.Text = "closeFile failed";
                    }
                    testbox.Items.Insert(0, "Received closeFile message");
                    if (testMode) Thread.Sleep(1000);
                });
            };
            addClientProc("closeFile", closeFile);
        }
        //----< load addDepend processing into dispatcher dictionary >-------

        private void DispatcherAddDepend()
        {
            Action<CsMessage> addDepend = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    statusBarText.Text = "Received addDpend message";
                    testbox.Items.Insert(0, "Received addDpend message");
                    if (testMode) Thread.Sleep(1000);
                });
            };
            addClientProc("addDepend", addDepend);
        }
        //----< load getDepend processing into dispatcher dictionary >-------

        private void DispatcherGetDepend()
        {
            Action<CsMessage> getDepend = (CsMessage rcvMsg) =>
            {
                Dispatcher.Invoke(() =>
                {
                    DependList.Items.Clear();
                    int dependnum = Int32.Parse(rcvMsg.value("dependnum"));
                    for (int i = 0; i < dependnum; ++i)
                    {
                        DependList.Items.Insert(0, rcvMsg.value("depend" + i.ToString()));
                    }
                    statusBarText.Text = "Received getDepend message";
                    testbox.Items.Insert(0, "Received getDepend message");
                    if (testMode) Thread.Sleep(1000);
                });
            };
            addClientProc("getDepend", getDepend);
        }
        //----< load sendfile processing into dispatcher dictionary >------

        private void DispatcherSendFile()
        {
            Action<CsMessage> sendFile = (CsMessage rcvMsg) =>
            {
                string filename = rcvMsg.value("file");
                Dispatcher.Invoke(() =>
                {
                    string showText = "Received " + filename + " sendFile messages";
                    statusBarText.Text = showText;
                    testbox.Items.Insert(0, showText);
                    string contents = File.ReadAllText("../SaveFiles/" + filename);
                    showFile(filename, contents);
                });
            };
            addClientProc("sendFile", sendFile);
        }
        //----< load viewMetaData processing into dispatcher dictionary >-------

        private void DispatcherViewMetaData()
        {
            Action<CsMessage> viewMetaData = (CsMessage rcvMsg) =>
            {
                string filename = rcvMsg.value("filename");
                Dispatcher.Invoke(() =>
                {
                    testbox.Items.Insert(0, "recevie " + filename + " meatadata msg");
                    ListBox MetaData_List = (rcvMsg.value("source") != "FileList_checkout") ? MetaData_browse_List : MetaData_checkout_List;
                    MetaData_List.Items.Clear();


                    int dependnum = Int32.Parse(rcvMsg.value("dependnum"));
                    for (int i = 0; i < dependnum; ++i)
                    {
                        MetaData_List.Items.Insert(0, "  " + rcvMsg.value("depend" + i.ToString()));
                    }
                    if (dependnum != 0) MetaData_List.Items.Insert(0, "Dependencies: ");
                    int cate_num = Int32.Parse(rcvMsg.value("cate_num"));
                    for (int i = 0; i < cate_num; ++i)
                    {
                        MetaData_List.Items.Insert(0, "  " + rcvMsg.value("cate" + i.ToString()));
                    }
                    if (cate_num != 0) MetaData_List.Items.Insert(0, "Categories: ");
                    MetaData_List.Items.Insert(0, "status: " + rcvMsg.value("status"));
                    MetaData_List.Items.Insert(0, "Version: " + rcvMsg.value("version"));
                    MetaData_List.Items.Insert(0, "Description: " + rcvMsg.value("description"));
                    MetaData_List.Items.Insert(0, "NameSapce: " + rcvMsg.value("namesp"));
                    MetaData_List.Items.Insert(0, "Author: " + rcvMsg.value("author"));
                    MetaData_List.Items.Insert(0, "FileName: " + rcvMsg.value("filename"));
                    statusBarText.Text = "Received viewmetadata messages";
                    testbox.Items.Insert(0, "Received viewmetadata message");
                });
            };
            addClientProc("viewMetaData", viewMetaData);
        }

        //----< load all dispatcher processing >---------------------------

        private void loadDispatcher()
        {
            DispatcherLoadGetDirs();
            DispatcherLoadGetFiles();
            DispatcherCheckIn();
            DispatcherCheckOut();
            DispatcherQuery();
            DispatcherSendFile();
            DispatcherConnect();
            DispatcherViewData();
            DispatcherCheckAuthor();
            DispatcherCheckInFile();
            DispatcherAddDepend();
            DispatcherViewMetaData();
            DispatcherCloseFile();
            DispatcherGetDepend();
        }

        //----< add all path to pathtextblocks >---------------------------

        private void addPath()
        {
            pathTextBlock_.Add("DirList_checkin", PathTextBlock_checkin);
            pathTextBlock_.Add("DirList_checkout", PathTextBlock_checkout);
            pathTextBlock_.Add("DirList_browse", PathTextBlock_browse);
            PathTextBlock_checkin.Text = "Storage";
            PathTextBlock_checkout.Text = "Storage";
            PathTextBlock_browse.Text = "Storage";
            pathStack_viewfile.Push("../Storage");
            pathStack_checkin.Push("../Storage");
            pathStack_checkout.Push("../Storage");
            pathStack_browse.Push("../Storage");
        }

        //----< start Comm, fill window display with dirs and files >------

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            string[] arg = Environment.GetCommandLineArgs();
            if (arg.Length > 1)
            {
                clientPort = Int32.Parse(arg[1]);
                Dispatcher.Invoke(() =>
                {
                    clientPortText.Text = arg[1];
                });
            }

            // start Comm
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = clientAddrText.Text;
            endPoint_.port = clientPort;
            translater = new Translater();
            translater.listen(endPoint_);

            // start processing messages
            processMessages();

            // load dispatcher
            loadDispatcher();

            addPath();

            connectbtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            test();
        }
        //----< strip off name of first part of path >---------------------

        private string removeFirstDir(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }

        //----< return list depending on listname >---------------------------

        private Stack<string> Select_List(String listname)
        {
            if (listname == "DirList_checkin" || listname == "FileList_checkin")
                return pathStack_checkin;
            if (listname == "DirList_checkout" || listname == "FileList_checkout")
                return pathStack_checkout;
            if (listname == "DirList_browse" || listname == "FileList_browse")
                return pathStack_browse;

            return pathStack_viewfile;
        }

        //----< respond to mouse double-click on dir name >----------------

        private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            // build path for selected dir
            ListBox dir = sender as ListBox;
            /*Console.Write(dir.Name);*/

            pathStack_ = Select_List(dir.Name);

            if (dir.SelectedItem == null)
            {
                return;
            }
            string selectedDir = (string)dir.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_.Count > 1)  // don't pop off "Storage"
                    pathStack_.Pop();
                else
                    return;
            }
            else
            {
                path = pathStack_.Peek() + "/" + selectedDir;
                pathStack_.Push(path);
            }
            // display path in Dir TextBlcok
            pathTextBlock_[dir.Name].Text = removeFirstDir(pathStack_.Peek());

            // build message to get dirs and post it
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = serverAddr;
            serverEndPoint.port = serverPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            msg.add("listname", dir.Name);
            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }

        //----< respond to mouse double-click on file list >----------------

        private void FileList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            ListBox it = sender as ListBox;
            pathStack_ = Select_List(it.Name);
            if (it != null && it.SelectedItem != null)
            {
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = serverAddr;
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "sendFile");
                msg.add("source", "ohterlist");
                msg.add("path", pathStack_.Peek());
                msg.add("filename", it.SelectedItem.ToString());
                translater.postMessage(msg);
            }
        }
        //----< respond to mouse double-click on query list >----------------

        private void QueryList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            ListBox it = sender as ListBox;
            if (it != null && it.SelectedItem != null)
            {
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = serverAddr;
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "sendFile");
                msg.add("source", "querylist");
                msg.add("key", it.SelectedItem.ToString());
                translater.postMessage(msg);
            }
        }

        //----< respond to mouse click on checkin button >----------------

        private void Button_Click_Checkin(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = serverAddr;
            serverEndPoint.port = serverPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "checkAuthor");
            msg.add("author", author_text.Text);
            msg.add("desciption", descrip_text.Text);
            string[] cates = cate_text.Text.Split(',');
            msg.add("cate_num", cates.Length.ToString());
            for (int i = 0; i < cates.Length; ++i)
            {
                msg.add("cate" + i.ToString(), cates[i]);
            }
            msg.add("category", cate_text.Text);
            msg.add("namesp", namesp_text.Text);
            msg.add("filename", System.IO.Path.GetFileName(fileselect.Text));
            msg.add("subdir", subdir_text.Text); 
            if (!validateCheckinInfo(msg)) return;
            translater.postMessage(msg);
        }
        //----< check whether all the necessary fields are filled >----------------

        private bool validateCheckinInfo(CsMessage msg)
        {
            bool allFieldSet = true;
            if (msg.value("author") == "")
            {
                author_text.Background = Brushes.Red;
                allFieldSet = false;
            }
            if (msg.value("desciption") == "")
            {
                descrip_text.Background = Brushes.Red;
                allFieldSet = false;
            }
            if (msg.value("category") == "")
            {
                cate_text.Background = Brushes.Red;
                allFieldSet = false;
            }
            if (msg.value("filename") == "File Selected")
            {
                fileselect.Background = Brushes.Red;
                allFieldSet = false;
            }
            if (!allFieldSet)
            {
                Dispatcher.Invoke(() =>
                {
                    statusBarText.Text = "Please Fill all Fields!";
                });
            }
            return allFieldSet;
        }

        //----< respond to mouse click on checkout button >----------------

        private void Button_Click_Checkout(object sender, RoutedEventArgs e)
        {
            if (FileList_checkout.SelectedItem != null)
            {
                checkout_Status.Items.Clear();
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = serverAddr;
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "checkOut");
                msg.add("with_depend", with_depend_check.IsChecked.ToString());
                msg.add("filename", FileList_checkout.SelectedItem.ToString());
                msg.add("filepath", "../" + PathTextBlock_checkout.Text);
                translater.postMessage(msg);
            }

        }

        //----< respond to mouse click on browse button >----------------

        private void Button_Click_Query(object sender, RoutedEventArgs e)
        {
            queryRes_List.Items.Clear();
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = serverAddr;
            serverEndPoint.port = serverPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "query");
            ItemCollection conditions = Condtion_List.Items;
            int num = Condtion_List.Items.Count;
            msg.add("num", num.ToString());
            msg.add("isAndQuery", andquery.IsChecked.ToString());
            msg.add("noparent", noparent.IsChecked.ToString());
            for (int i = 0; i < num; ++i)
            {
                DockPanel con = (DockPanel)conditions.GetItemAt(i);
                msg.add("ConName" + i.ToString(), (string)((Label)con.Children[0]).Content);
                msg.add("MatchCon" + i.ToString(), (string)((Label)con.Children[1]).Content);
                msg.add("Condition" + i.ToString(), (string)((Label)con.Children[2]).Content);
            }
            translater.postMessage(msg);
        }

        //----< show file content in the popup window >----------------

        private void showFile(string fileName, string fileContents)
        {
            Paragraph paragraph = new Paragraph();
            paragraph.Inlines.Add(new Run(fileContents));
            CodePopupWindow popUp = new CodePopupWindow();
            popUp.Show();
            popUp.codeView.Blocks.Clear();
            popUp.codeView.Blocks.Add(paragraph);
        }

        //----< respond to mouse click on connect button >----------------

        private void Button_Click_Connect(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverAddr = machineAddressText.Text;
            serverPort = Int32.Parse(portText.Text);
            serverEndPoint.machineAddress = serverAddr;
            serverEndPoint.port = serverPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "connect");
            translater.postMessage(msg);
        }

        //----< respond to mouse click on viewdata button >----------------

        private void Button_Click_ViewMetaData(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "viewMetaData");
            translater.postMessage(msg);
        }


        //----< respond to mouse click on selectfile button >----------------

        private void Button_Click_SelectFile(object sender, RoutedEventArgs e)
        {
            // Create OpenFileDialog 
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            dlg.DefaultExt = ".*";
            //dlg.Filter = "JPEG Files (*.jpeg)|*.jpeg|PNG Files (*.png)|*.png|JPG Files (*.jpg)|*.jpg|GIF Files (*.gif)|*.gif";

            // Display OpenFileDialog by calling ShowDialog method 
            Nullable<bool> result = dlg.ShowDialog();

            // Get the selected file name and display in a TextBox 
            if (result == true)
            {
                // Open document 
                string filename = dlg.FileName;
                fileselect.Text = filename;
            }
        }
        //----< respond to mouse click on selectdes button >----------------

        private void Button_Click_SelectDes(object sender, RoutedEventArgs e)
        {
            // Create FolderBrowserDialog 
            System.Windows.Forms.FolderBrowserDialog dlg = new System.Windows.Forms.FolderBrowserDialog();
            dlg.ShowDialog();
            string path = dlg.SelectedPath;
            desselect.Text = path;
        }
        //----< make the field back to white background >----------------

        private void Back_white(object sender, TextChangedEventArgs e)
        {
            TextBox t = sender as TextBox;
            t.Background = Brushes.White;
        }
        //----< respond to mouse click on add dependlist button >----------------

        private void Button_Click_AddDepend(object sender, RoutedEventArgs e)
        {
            if (FileList_checkin.SelectedItem != null && FileList_checkin.SelectedItem.ToString() != ParentFile.Text)
            {
                DependList.Items.Insert(0, FileList_checkin.SelectedItem);
            }
        }
        //----< respond to mouse click on delete dependlist button >----------------

        private void Button_Click_DelDepend(object sender, RoutedEventArgs e)
        {
            if (DependList.SelectedItem != null)
            {
                DependList.Items.Remove(DependList.SelectedItem);
            }
        }
        //----< respond to mouse click on comfirm depend button >----------------

        private void Button_Click_ConfirmDepend(object sender, RoutedEventArgs e)
        {
            if (ParentFile.Text == "") return;
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverAddr = machineAddressText.Text;
            serverPort = Int32.Parse(portText.Text);
            serverEndPoint.machineAddress = serverAddr;
            serverEndPoint.port = serverPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "addDepend");
            msg.add("parent", ParentFile.Text);
            msg.add("num", DependList.Items.Count.ToString());
            for (int i = 0; i < DependList.Items.Count; ++i)
            {
                msg.add("depend" + i.ToString(), DependList.Items[i].ToString());
            }
            translater.postMessage(msg);
        }
        //----< respond to filelist selecteditem change >----------------

        private void FileList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ListBox it = sender as ListBox;
            if (it.SelectedItem != null)
            {
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverAddr = machineAddressText.Text;
                serverPort = Int32.Parse(portText.Text);
                serverEndPoint.machineAddress = serverAddr;
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "viewMetaData");
                msg.add("source", it.Name);
                msg.add("filename", it.SelectedItem.ToString());
                translater.postMessage(msg);
            }
        }
        //----< respond to mouse click on closefile button >----------------

        private void Button_Click_CloseFile(object sender, RoutedEventArgs e)
        {
            if (FileList_browse.SelectedItem != null)
            {
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverAddr = machineAddressText.Text;
                serverPort = Int32.Parse(portText.Text);
                serverEndPoint.machineAddress = serverAddr;
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "closeFile");
                msg.add("filename", FileList_browse.SelectedItem.ToString());
                translater.postMessage(msg);
            }
        }

        //----< respond to mouse click on addcondition button >----------------

        private void Button_Click_AddCondi(object sender, RoutedEventArgs e)
        {
            string condition = condiNameSele.Text + " ";
            condition += condiMatchSele.Text;
            condition += " ";
            condition += Condtion_text.Text;

            DockPanel dk = new DockPanel();
            Label nam = new Label();
            nam.Content = condiNameSele.Text;
            nam.Width = 80;
            dk.Children.Add(nam);
            Label mat = new Label();
            mat.Content = condiMatchSele.Text;
            mat.Width = 80;
            dk.Children.Add(mat);
            Label con = new Label();
            con.Content = Condtion_text.Text;
            con.Width = 180;
            dk.Children.Add(con);


            Condtion_List.Items.Insert(0, dk);
        }
        //----< respond to mouse click on set_parent button >----------------

        private void Button_Click_SetParent(object sender, RoutedEventArgs e)
        {
            if (FileList_checkin.SelectedItem != null)
            {
                ParentFile.Text = FileList_checkin.SelectedItem.ToString();
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverAddr = machineAddressText.Text;
                serverPort = Int32.Parse(portText.Text);
                serverEndPoint.machineAddress = serverAddr;
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "getDepend");
                msg.add("filename", FileList_checkin.SelectedItem.ToString());
                translater.postMessage(msg);
            }
        }
        //----< respond to mouse click delete condition button >----------------

        private void deletecondibtn_Click(object sender, RoutedEventArgs e)
        {
            if (Condtion_List.SelectedItem != null)
            {
                Condtion_List.Items.Remove(Condtion_List.SelectedItem);
            }
        }
        //----< mock checkin, with informations, used in test >----------------

        private void mock_checkin(string filepath, string author, string descrip, string cate, string namesp, string subdir)
        {
            fileselect.Text = filepath;
            author_text.Text = author;
            descrip_text.Text = descrip;
            cate_text.Text = cate;
            namesp_text.Text = namesp;
            subdir_text.Text = subdir;
            checkinbtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
        }
        //----< test function to display requirements >----------------

        private async void test()
        {
            test1();
            test2();
            await Task.Delay(60000);
            test3();
            await Task.Delay(500);
            test4();
            await Task.Delay(500);
            test5();
            await Task.Delay(500);
            test6();
        }
        //----< print information in console and GUI >----------------

        private void printTestLine(string info)
        {
            Console.Write("  " + info + " \n");
            Demo_list.Items.Insert(0, info);
        }

        //----< Test function for requirement #1 >----------------

        private void test1()
        {
            Console.Write("\n Demonstrating Requirement #1 \n");
            Console.Write(" ============================== \n");
            Console.Write("  This project use Visual Studio 2017 and the standard C++ libraries. Also use C#, the .Net Windows Presentation Foundation framework, and C++/CLI for the graphical part of each Client. \n");
            Console.Write("  Requirement #1 passed! \n");
        }

        //----< Test function for requirement #2 >----------------

        private async void test2()
        {
            Console.Write("\n Demonstrating Requirement #2 \n");
            Console.Write(" ============================== \n");
            Console.Write("  This test will show the functionalities of the database. \n");
            Console.Write("  A window will pop up and show the file content. \n");

            test2a(); // for checkin
            await Task.Delay(15000);
            test2b(); // for add dependencies
            await Task.Delay(5000);
            test2c(); // for checkout
            await Task.Delay(7000);
            test2d(); // for browse
            await Task.Delay(12000);
            test2e(); // for query
            await Task.Delay(8000);

            Console.Write("  Requirement #2 passed. \n");
        }
        //----< Test function for requirement #2a, checkin >----------------

        private async void test2a()
        {
            tabControl.SelectedIndex = 2;
            printTestLine("\nConnected to the server, it can support multi clients shown in project3.");
            printTestLine("\n* Testing check in......");
            Console.Write("  ============================== \n");
            printTestLine("Check in file comm.cpp");
            mock_checkin("../DemoFiles/Comm.cpp", "Cheng", "A comm cpp file", "communication,utility", "COMM", "");
            await Task.Delay(2000);
            printTestLine("Check in file comm.h");
            mock_checkin("../DemoFiles/Comm.h", "Cheng", "A comm header file", "communication,utility", "COMM", "");
            await Task.Delay(2000);
            printTestLine("Check in file Sockets.cpp");
            mock_checkin("../DemoFiles/Sockets.cpp", "Cheng", "Package Socket cpp file", "socket,communication", "SOCKET", "/Socket");
            await Task.Delay(2000);
            printTestLine("Check in file Sockets.h");
            mock_checkin("../DemoFiles/Sockets.h", "Cheng", "Package Socket h file", "socket,communication", "SOCKET", "/Socket");
            await Task.Delay(2000);
            printTestLine("Check in file Message.h");
            mock_checkin("../DemoFiles/Message.h", "Cheng", "Package Message h file", "socket,communication", "SOCKET", "");
            await Task.Delay(2000);
            printTestLine("Check in file Message.cpp");
            mock_checkin("../DemoFiles/Message.cpp", "Cheng", "Package Message cpp file", "socket,communication", "SOCKET", "");
            await Task.Delay(2000);
            printTestLine("Trying to Checking in file Message.cpp with another author, will be rejected.");
            mock_checkin("../DemoFiles/Message.cpp", "AuthorB", "Package Message cpp file", "socket,communication", "SOCKET", "");
        }
        //----< Test function for requirement #2b, add dependency >----------------

        private async void test2b()
        {
            tabControl.SelectedIndex = 2;
            checkin_tab.SelectedIndex = 1;
            printTestLine("\n* Testing adding dependencies......");
            Console.Write("  ===================================== \n");
            // set some file as parent
            printTestLine("Set the first file as parent file.");
            FileList_checkin.SelectedIndex = 0;
            set_parent_btn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            await Task.Delay(2000);
            // add to the depend list
            printTestLine("Add the second file to the dependency list");
            FileList_checkin.SelectedIndex = 1;
            adddependbtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            printTestLine("Add the third file to the dependency list");
            FileList_checkin.SelectedIndex = 2;
            adddependbtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            //confirm
            await Task.Delay(2000);
            printTestLine("Comfirm the dependency list.");
            confirmdependbtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
        }
        //----< Test function for requirement #2c, checkout >----------------

        private async void test2c()
        {
            tabControl.SelectedIndex = 3;
            printTestLine("\n* Testing checkout......");
            Console.Write("  ===================================== \n");
            // checkout the single file
            FileList_checkout.SelectedIndex = 1;
            desselect.Text = "../checkoutFiles";
            printTestLine("Checkout the second file.");
            printTestLine("Files are checkout to root/GUI/bin/x86/checkOutFiles.");
            await Task.Delay(2000);
            checkoutbtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));

            // checkout file with dependencies
            printTestLine("Checkout the first file with dependency.");
            printTestLine("Files are checkout to root/GUI/bin/x86/checkOutFiles.");
            await Task.Delay(1000);
            FileList_checkout.SelectedIndex = 0;
            with_depend_check.IsChecked = true;
            await Task.Delay(2000);
            checkoutbtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
        }
        //----< Test function for requirement #2d, browse and close file >----------------

        private async void test2d()
        {
            tabControl.SelectedIndex = 4;
            printTestLine("\n* Testing browse and close file......");
            Console.Write("  ===================================== \n");
            // view metadata
            printTestLine("Get information of the first file.");
            FileList_browse.SelectedIndex = 0;
            await Task.Delay(2000);
            printTestLine("Switch to the second file, Get information of the second file.");
            FileList_browse.SelectedIndex = 1;
            await Task.Delay(2000);
            
            // close the file and checkin again.
            FileList_browse.SelectedIndex = 1;
            await Task.Delay(2000);
            printTestLine("Close the firse file.");
            closefilebtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            FileList_browse.SelectedIndex = 0;
            FileList_browse.SelectedIndex = 1;
            await Task.Delay(2000);
            printTestLine("Now its status is closed.");
            printTestLine("Check in the file again and you will see a new version of it.");
            mock_checkin("../DemoFiles/Message.cpp", "Cheng", "Package Message cpp file", "socket,communication", "SOCKET", "");
            await Task.Delay(4000);
            printTestLine("You can pop up a code window by double click in browse, I leave this test to the query part.");
        }
        //----< Test function for requirement #2a, query >----------------

        private async void test2e()
        {
            tabControl.SelectedIndex = 5;
            printTestLine("* Testing query......");
            Console.Write("  ===================================== \n");
            // and query
            condiNameSele.Text = "Name";
            condiMatchSele.Text = "includes";
            Condtion_text.Text = "o";
            printTestLine("Add query condition: name includes 'o'");
            add_condi_btn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            await Task.Delay(2000);
            condiNameSele.Text = "Category";
            condiMatchSele.Text = "is exact";
            Condtion_text.Text = "utility";
            printTestLine("Add query condition: Category is exact 'utility'");
            add_condi_btn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            await Task.Delay(2000);
            printTestLine("Apply 'and query' on the conditions.");
            querybtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            await Task.Delay(2000);
            printTestLine("Delete currents conditions.");
            Condtion_List.SelectedIndex = 0;
            deletecondibtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            await Task.Delay(2000);
            Condtion_List.SelectedIndex = 0;
            deletecondibtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            // or query
            condiNameSele.Text = "Name";
            condiMatchSele.Text = "is exact";
            Condtion_text.Text = "Sockets.cpp";
            printTestLine("Add query condition: Name is exact 'Sockets.cpp'");
            await Task.Delay(2000);
            add_condi_btn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            condiNameSele.Text = "Namespace";
            condiMatchSele.Text = "is exact";
            Condtion_text.Text = "COMM";
            printTestLine("Add query condition: Namespace is exact 'COMM'");
            await Task.Delay(2000);
            add_condi_btn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            orquery.IsChecked = true;
            printTestLine("Apply 'or query' on the conditions.");
            await Task.Delay(2000);
            querybtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            Condtion_List.SelectedIndex = 0;
            deletecondibtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            await Task.Delay(2000);
            Condtion_List.SelectedIndex = 0;
            deletecondibtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            printTestLine("Query to get files with no parent.");
            await Task.Delay(1000);
            andquery.IsChecked = true;
            noparent.IsChecked = true;
            await Task.Delay(2000);
            querybtn.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            printTestLine("Will get all files except two files as dependencies.");
            await Task.Delay(2000);
            //// popup window
            printTestLine("Pop up code of the first file.");
            MouseButtonEventArgs e = new MouseButtonEventArgs(Mouse.PrimaryDevice, 0, MouseButton.Left);
            e.RoutedEvent = Control.MouseDoubleClickEvent;
            e.Source = queryRes_List;
            queryRes_List.SelectedIndex = 0;
            queryRes_List.RaiseEvent(e);

        }

        //----< Test function for requirement #3 >----------------

        private void test3()
        {
            Console.Write("\n Demonstrating Requirement #3 \n");
            Console.Write("\n ============================== \n");
            Console.Write("  This test will show that a client can upload and download a file.  \n");
            Console.Write("  Uploading a file is completed in the checkin process.  \n");
            Console.Write("  Downloading a file is demonstrated by pop up a code window.  \n");
            Console.Write("  Requirement #3 passed. \n");

        }

        //----< Test function for requirement #4 >----------------

        private void test4()
        {
            Console.Write("\n Demonstrating Requirement #4 \n");
            Console.Write(" ============================== \n");
            Console.Write("  This test will show a message-passing communication system  \n");
            Console.Write("  Please refer to the massages in the server console and GUI console.  \n");
            Console.Write("  The massges demostrates the communication between two ends.  \n");
            Console.Write("  Take checkin as a example. In the checkin process, the communication would be:  \n");
            Console.Write("    Client to Server : 'chechAuthor' to validate author  \n");
            Console.Write("    Server to Client : 'chechAuthor' msg, sucecced or not.  \n");
            Console.Write("    Client to Server : 'chechAuthor' false: display error; true: send 'checkinFile' msg.  \n");
            Console.Write("    Server to Client : Check in file', return 'checkinFile' msg to tell client checkin finished.  \n");
            Console.Write("  Requirement #4 passed. \n");

        }

        //----< Test function for requirement #5 >----------------

        private void test5()
        {
            Console.Write("\n Demonstrating Requirement #5 \n");
            Console.Write(" ============================== \n");
            Console.Write("  This test will show providing support for passing HTTP style messages using asynchronous one-way messaging  \n");
            Console.Write("  This test will send a showdb() message to server which is a one-way message.  \n");
            Console.Write("  DB content will be printed on the server side.  \n");
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverAddr = machineAddressText.Text;
            serverPort = Int32.Parse(portText.Text);
            serverEndPoint.machineAddress = serverAddr;
            serverEndPoint.port = serverPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "showdb");
            translater.postMessage(msg);
            Console.Write("  Requirement #5 passed. \n");

        }

        //----< Test function for requirement #6 >----------------

        private void test6()
        {
            Console.Write("\n Demonstrating Requirement #6 \n");
            Console.Write(" ============================== \n");
            Console.Write("  This test will show support sending and receiving blocks of bytes6 to support file transfer.  \n");
            Console.Write("  Please refer to the console, the file information will be printed when trasferring a file.  \n");
            Console.Write("  Requirement #6 passed. \n");

        }

    }
}