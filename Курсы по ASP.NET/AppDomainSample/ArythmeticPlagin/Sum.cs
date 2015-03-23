using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DomainsDemo.PluginInterface;

namespace ArythmeticPlugin
{
    [Plugin("Sum")]
	[Serializable]
    public class Sum : PluginBase
    {
        public override double Operation(double[] input)
        {
            return input.Sum();
        }
    }
}
