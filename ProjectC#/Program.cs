﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using Editor.Forms;

namespace Editor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            NDYEditor form = new NDYEditor();
            form.Show();
            form.GameLoop();
        }
    }
}
