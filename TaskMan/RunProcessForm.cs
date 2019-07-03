using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TaskMan
{
    public partial class RunProcessForm : Form
    {
        public string FileName { get => textBox1.Text; }

        public RunProcessForm()
        {
            InitializeComponent();
            Init();
        }

        private void Init()
        {
            ActiveControl = textBox1;
        }

        private void RunProcessForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == Keys.Enter)
            {
                this.DialogResult = DialogResult.OK;
            }
            else if (e.KeyData == Keys.Escape)
            {
                this.DialogResult = DialogResult.Cancel;
            }
        }
    }
}
