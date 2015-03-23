using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DomainsDemo.PluginInterface
{
	[Serializable]
    public abstract class PluginBase : IPlugin
    {
		public abstract double Operation( double[] input );

        public string Name
        {
            get
            {
                var attributes = (PluginAttribute[])this.GetType().GetCustomAttributes(typeof(PluginAttribute), false);
                return string.Format("Context domain: {0}. Class: {1}.", AppDomain.CurrentDomain.FriendlyName, attributes[0].Name);
            }
        }

    }
}
