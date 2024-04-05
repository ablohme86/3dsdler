using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Net;

class Program
{
    static void Main(string[] args)
    {
        string search_term = null;
        string region = null;
        string ignore_term = null;

        ParseArguments(args, ref search_term, ref region, ref ignore_term);

        if (search_term == null || region == null)
        {
            Console.WriteLine("Bruk: {0} --wildcard=<nøkkelord> --region=<region> [--ignore=<nøkkelord>]", AppDomain.CurrentDomain.FriendlyName);
            return;
        }

        string prefix = "https://myrient.erista.me/files/No-Intro/Nintendo%20-%20Nintendo%203DS%20(Decrypted)";
        int files_found = 0;

        using (StreamReader file = new StreamReader("dblist"))
        {
            string line;
            while ((line = file.ReadLine()) != null)
            {
                if (line.Contains(search_term))
                {
                    if (ignore_term != null && line.Contains(ignore_term))
                    {
                        continue;
                    }

                    Match hrefMatch = Regex.Match(line, @"href=\""([^\""]*)\""", RegexOptions.IgnoreCase);
                    if (hrefMatch.Success)
                    {
                        string ptr = hrefMatch.Groups[1].Value;
                        string filename = ptr + ".zip";

                        if (filename.Contains(region))
                        {
                            Match nameMatch = Regex.Match(line, @">(.*?)</a>", RegexOptions.IgnoreCase);
                            if (nameMatch.Success)
                            {
                                string name = nameMatch.Groups[1].Value;
                                Console.WriteLine("Laster ned: {0}", name);
                            }
                            else
                            {
                                Console.WriteLine("Laster ned: {0}", filename);
                            }

                            string downloadUrl = $"{prefix}/{ptr}";
                            using (WebClient client = new WebClient())
                            {
                               // Console.WriteLine($"Trying to get url: {downloadUrl}");
                                client.DownloadFile(downloadUrl, $"{filename}");
                            }

                            files_found++;
                        }
                    }
                }
            }
        }

        if (files_found == 0)
        {
            Console.WriteLine("Finner ingen filer ved oppgitte parametere.");
        }
    }

    static void ParseArguments(string[] args, ref string search_term, ref string region, ref string ignore_term)
    {
        foreach (string arg in args)
        {
            if (arg.StartsWith("--wildcard="))
            {
                search_term = arg.Substring(11);
            }
            else if (arg.StartsWith("--region="))
            {
                region = arg.Substring(9);
            }
            else if (arg.StartsWith("--ignore="))
            {
                ignore_term = arg.Substring(9);
            }
        }
    }
}

