using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DomainsDemo
{
	public class Action
	{
		public string Operation { get; private set; }
		public double[] Data 
		{ 
			get
			{
				double[] data = (double[])_data.Clone();
				return data;
			}

			private set
			{
				_data = value;
			}
		}

		public Action( string operation, double[] data )
		{
			this.Operation = operation;
			this.Data = data;
		}

		private double[] _data = null;
	}
}
