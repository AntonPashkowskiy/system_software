using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DomainsDemo.PluginInterface;

namespace DomainsDemo
{
	public class AsynchOperationArguments
	{
		public string Class { get; private set; }
		public IPlugin Instance { get; private set; }
		public double[] Data
		{
			get
			{
				double[] data = ( double[] )_data.Clone();
				return data;
			}

			private set
			{
				_data = value;
			}
		}

		public AsynchOperationArguments( string className, IPlugin instance, double[] data )
		{
			this.Class = className;
			this.Instance = instance;
			this.Data = data;
		}

		private double[] _data = null;
	}
}
