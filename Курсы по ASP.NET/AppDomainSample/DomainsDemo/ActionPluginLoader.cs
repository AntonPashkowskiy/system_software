using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace DomainsDemo
{
	public static class ActionPluginLoader
	{
		public static List<ActionPlugin> Load( string pathToFile )
		{
			if ( pathToFile == null || Path.GetExtension( pathToFile ) != ".xml" )
			{
				throw new ArgumentException( "Путь к файлу не задан или файл не является .xml документом." );
			}
				
			try
			{
				List<ActionPlugin> resultList = new List<ActionPlugin>();
				XDocument actionPluginDoc = XDocument.Load( pathToFile );

				resultList = (from element in actionPluginDoc.Descendants( "ActionPlugin" )
							  select new ActionPlugin(
								  element.Element( "Operation" ).Value,
								  element.Element( "Class" ).Value,
								  element.Element( "Assembly" ).Value
							  )).ToList();

				if ( resultList.Count == 0 )
				{
					return null;
				}
				else
				{
					return resultList;
				}
			}
			catch( FileNotFoundException )
			{
				throw new FileNotFoundException( "Файл xml не найден." );
			}
		}
	}
}
