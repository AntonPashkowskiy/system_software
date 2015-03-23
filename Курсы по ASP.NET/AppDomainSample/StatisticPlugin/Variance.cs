using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DomainsDemo.PluginInterface;

namespace StatisticPlugin
{
	[Serializable]
    [Plugin( "Variance", Complexity.Quadratic ,true )]
    public class Variance : PluginBase
    {
        public override double Operation(double[] input)
        {
            double sumSquares = input.Sum(t => t * t);
            double mean = new Mean().Operation( input );
            return (sumSquares - input.Length * mean * mean) / (input.Length - 1);
        }
    }
}
