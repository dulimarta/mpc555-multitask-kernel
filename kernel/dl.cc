#include <algorithm>
#include <numeric>
#include <functional>
#include <stdlib.h>
#include <slist>

typedef pair<char,int> Timer;

slist <Timer> S;
int n_timer = 0;

void SetAlarm (Timer n)
{
   int cv, k, c = n.second;
   slist<Timer>::iterator p, pp;

   cout << "Inserting new count: " << n.second << endl;
   cv = 0;
   p = S.begin();
   pp = 0;
   k = 0;
   while (k < n_timer) {
      cv += p->second;
      if (c < p->second)
         break;
      c -= p->second;
      //cout << "Skip " << p->second << " key is " << c << endl;
      pp = p;
      p++;
      k++;
   }
   n.second = c;
   if (pp == 0) {
      //cout << "Inserting " << c << " before " << cv << endl;
      S.push_front (n);
   }
   else {
      S.insert_after (pp, n);
   }
   if (p != 0) p->second -= c;
   n_timer++;
}

template <int MAX>
Timer my_rand ()
{
   return make_pair('X', 
      1 + (int) (float(MAX) * rand()/(RAND_MAX+1.0)));
}

int my_irand(int MAX)
{
   return 1 + (int) (float(MAX) * rand()/(RAND_MAX+1.0));
}

void out (const slist<Timer>& x)
{
   slist<int>T (x.size());

   transform (x.begin(), x.end(), T.begin(), select2nd<Timer>());
   //copy (T.begin(), T.end(), ostream_iterator<int>(cout, " "));
   //cout << endl;
   partial_sum (T.begin(), T.end(), T.begin());
   copy (T.begin(), T.end(), ostream_iterator<int>(cout, " "));
   cout << endl;
}

main()
{
   //generate_n (S.begin(), 10, my_rand<30>);
   //out(S);
   int k = 0;
   while (k < 20) {
      SetAlarm (make_pair ('A' + k, my_irand (300)));
      out (S);
      k++;
   }
}

