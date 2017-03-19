using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Windows.Media.SpeechRecognition;
using Windows.Globalization;

namespace VoiceDesktopApp
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		SpeechRecognizer recognizer = new SpeechRecognizer(new Language("en-US"));
		

		private void Form1_Load(object sender, EventArgs e)
		{

		}

		private async void button1_Click(object sender, EventArgs e)
		{
			result =
					await recognizer.CompileConstraintsAsync();
		}
	}
}
