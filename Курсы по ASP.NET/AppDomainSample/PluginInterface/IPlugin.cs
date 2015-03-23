using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DomainsDemo.PluginInterface
{
    public interface IPlugin
    {
        string Name { get; }
        double Operation( double[] input );
    }
}
