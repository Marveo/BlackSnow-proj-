class KTQConst
{
	static const string ROOT_DIR = "$profile:Zadachi_profiles";
	static const string QUESTS_DIR = ROOT_DIR + "\\Quests\\";
	static const string DB_DIR = ROOT_DIR + "\\db\\";
	static const string ACTIVEQ_DIR = ROOT_DIR + "\\Active\\";
	static const string CONFIG = ROOT_DIR + "\\Config.json";

	static void ScanFolderForQuests(string path, out array<string> list)
	{
		string	file_name;
		int 	file_attr;
		int		flags;
		
		string path_find_pattern = path + "*.json";
		FindFileHandle file_handler = FindFile(path_find_pattern, file_name, file_attr, flags);
		
		bool found = true;
		while ( found )
		{				
			list.Insert(file_name);
			
			found = FindNextFile(file_handler, file_name, file_attr);
		}
	}
}