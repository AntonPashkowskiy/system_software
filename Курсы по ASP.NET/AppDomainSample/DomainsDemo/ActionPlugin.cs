using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DomainsDemo
{
	public class ActionPlugin
	{
		public string Operation { get; private set; }
		public string Class { get; private set; }
		public string Assembly { get; private set; }

		public ActionPlugin( string Operation, string Class, string Assembly  )
		{
			this.Operation = Operation;
			this.Class = Class;
			this.Assembly = Assembly;
		}
	}
}
