namespace dalisearch;
class Program
{

    static List<int> addr = new List<int>()
    {
        0xFF3300,
        0x01e41b,
        0xFEDCBA,
        0x123456,
        0x001122
    };

    static List<int> ballasts = new List<int>();

    static void Main(string[] args)
    {
        Console.WriteLine("Hello, World!");
        
        int low = 0;
        int high = 0xffffff;
        while(low > -1)
        {
            low = find_next(low, high);
            if(low > -1)
            {
                ballasts.Add(low);
                low += 1;
            }
        }

        
        System.Console.WriteLine("Found {0}", ballasts.Count);

        foreach(int ballast in ballasts)
        {
            System.Console.WriteLine(" - {0:X6}", ballast);
        }
    }
    
    static int find_next(int low, int high)
    {
        bool response = false;
        System.Console.WriteLine("Searching from {0:X6} to {1:X6}...", low, high);
        if(low == high)
        {
            response = addr.Any(a => a <= low);

            if(response)
            {
                System.Console.WriteLine("Found ballast at {0:X6}; withdrawing it...", low);
                addr.Remove(low);
                //i.send(Withdraw())
                return low;
            }
            return -1;
        }

        response = addr.Any(a => a <= high);

        if(response)
        {
            int midpoint = (low + high) / 2;
            int resp = find_next(low, midpoint);
            if(resp > -1) return resp;
            return find_next(midpoint + 1, high);
        }

        return -1;
    }
}
