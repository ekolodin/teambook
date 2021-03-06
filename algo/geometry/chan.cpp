#include <bits/stdc++.h>
using namespace std;
#define forn(i,n) for (int i = 0; i < int(n); ++i)
#define sz(x) ((int) (x).size())
#define all(x) x.begin(), x.end()
typedef long double ld;

const int maxn = 100100;

struct pt {
    ld x, y, z;
    int id, pr, nx;
    bool onHull;

    pt() {}

    pt(ld x, ld y, ld z): x(x), y(y), z(z) {}

    pt operator-(const pt &p) const {
        return pt(x - p.x, y - p.y, z - p.z);
    }

    ld operator*(const pt &p) const {
        return x * p.x + y * p.y + z * p.z;
    }

    pt operator%(const pt &p) const {
        return pt(y * p.z - z * p.y,
                  z * p.x - x * p.z,
                  x * p.y - y * p.x);
    }
};

//BEGIN_CODE
mt19937 rr(111);
ld rndEps() {
    return (ld(rr()) / rr.max() - 0.5) * 1e-7;
}

typedef tuple<int, int, int> Face;
const ld infc = 1e100;

int n;
pt p[maxn];

namespace Chan {
pt _p[maxn];

ld turny(int p1, int p2, int p3) {
    return (p[p2].x - p[p1].x) * (p[p3].y - p[p1].y) - 
        (p[p3].x - p[p1].x) * (p[p2].y - p[p1].y);
}

//replace y with z
ld turnz(int p1, int p2, int p3) {
    return (p[p2].x - p[p1].x) * (p[p3].z - p[p1].z) - 
        (p[p3].x - p[p1].x) * (p[p2].z - p[p1].z);
}

ld gett(int p1, int p2, int p3) {
    if (p1 == -1 || p2 == -1 || p3 == -1)
        return infc;
    ld ty = turny(p1, p2, p3);
    if (ty >= 0)
        return infc;
    else
        return turnz(p1, p2, p3) / ty;
}

void act(int i) {
    if (p[i].onHull) {
        p[p[i].nx].pr = p[i].pr;
        p[p[i].pr].nx = p[i].nx;
    } else {
        p[p[i].nx].pr = p[p[i].pr].nx = i;
    }
    p[i].onHull ^= 1;
}

ld updt(vector<int> &V) {
    if (V.empty())
        return infc;
    int id = V.back();
    if (p[id].onHull)
        return gett(p[id].pr, p[id].nx, id);
    else
        return gett(p[id].pr, id, p[id].nx);
}

//builds lower hull
vector<int> buildHull(int l, int r) {
    if (l + 1 >= r) {
        p[l].pr = p[l].nx = -1;
        p[l].onHull = true;
        return {};
    }
    int mid = (l + r) / 2;
    auto L = buildHull(l, mid);
    auto R = buildHull(mid, r);
    reverse(all(L));
    reverse(all(R));
    int u = mid - 1, v = mid;
    while (true) {
        if (p[u].pr != -1 && 
                (turny(p[u].pr, u, v) <= 0))
            u = p[u].pr;
        else if (p[v].nx != -1 && 
                (turny(u, v, p[v].nx) <= 0))
            v = p[v].nx;
        else
            break;
    }

    ld t[6];
    t[0] = updt(L);
    t[1] = updt(R);
    vector<int> A;
    while (true) {
        t[2] = gett(p[u].pr, v, u);
        t[3] = gett(u, p[u].nx, v);
        t[4] = gett(u, p[v].pr, v);
        t[5] = gett(u, p[v].nx, v);
        ld nt = infc;
        int type = -1;
        forn (i, 6)
            if (t[i] < nt)
                nt = t[i], type = i;
        if (nt >= infc)
            break;

        if (type == 0) {
            act(L.back());
            if (L.back() < u)
                A.push_back(L.back());
            L.pop_back();
            t[0] = updt(L);
        } else if (type == 1) {
            act(R.back());
            if (R.back() > v)
                A.push_back(R.back());
            R.pop_back();
            t[1] = updt(R);
        } else if (type == 2) {
            A.push_back(u);
            u = p[u].pr;
        } else if (type == 3) {
            A.push_back(u = p[u].nx);
        } else if (type == 4) {
            A.push_back(v = p[v].pr);
        } else if (type == 5) {
            A.push_back(v);
            v = p[v].nx;
        }
    }
    assert(L.empty() && R.empty());

    p[u].nx = v, p[v].pr = u;
    for (int i = u + 1; i < v; ++i)
        p[i].onHull = false;
    for (int i = sz(A) - 1; i >= 0; --i) {
        int id = A[i];
        if (id <= u || id >= v) {
            if (u == id)
                u = p[u].pr;
            if (v == id)
                v = p[v].nx;
            act(id);
        } else {
            p[id].pr = u, p[id].nx = v;
            act(id);
            if (id >= mid)
                v = id;
            else
                u = id;
        }
    }
    return A;
}

//faces are oriented ccw if look from the outside
vector<Face> getFaces() {
    forn (i, n) {
        _p[i] = p[i];
        p[i].x += rndEps();
        p[i].y += rndEps();
        p[i].z += rndEps();
        p[i].id = i;
    }
    sort(p, p + n, [](const pt &a, const pt &b) {
                return a.x < b.x;
            });
    vector<Face> faces;
    forn (q, 2) {
        auto movie = buildHull(0, n);
        for (int x: movie) {
            int id = p[x].id;
            int pid = p[p[x].pr].id;
            int nid = p[p[x].nx].id;
            if (!p[x].onHull)
                faces.emplace_back(pid, id, nid);
            else
                faces.emplace_back(pid, nid, id);
            act(x);
        }
        forn (i, n) {
            p[i].y *= -1;
            p[i].z *= -1;
        }
    }
    forn (i, n)
        p[i] = _p[i];
    return faces;
}

} //namespace Chan
//END_CODE

int main() {
#ifdef LOCAL
    assert(freopen("test.in", "r", stdin));
#endif
    int tn;
    cin >> tn;
    forn (i, tn) {
        cin >> n;
        forn (i, n)
            cin >> p[i].x >> p[i].y >> p[i].z;
        auto faces = Chan::getFaces();
        cerr << "time " << clock() / 1000 << '\n';
        vector<int> nodes;
        for (auto f: faces) {
            nodes.push_back(get<0>(f));
            nodes.push_back(get<1>(f));
            nodes.push_back(get<2>(f));
        }
        sort(all(nodes));
        nodes.erase(unique(all(nodes)), nodes.end());
        assert(sz(nodes) * 2 == sz(faces) + 4);
        cerr << "nodes " << sz(nodes) << '\n';
        cerr << "faces " << sz(faces) << '\n';
        ld V = 0, S = 0;
        for (auto f: faces) {
            pt v1 = p[get<1>(f)] - p[get<0>(f)];
            pt v2 = p[get<2>(f)] - p[get<0>(f)];
            pt v3 = p[get<0>(f)];
            pt vv = v1 % v2;
            /*
            forn (i, n) {
                pt v4 = p[i] - p[get<0>(f)];
                assert(v4 * vv < 0.001);
            }
            */
            S += sqrtl(vv * vv) / 2;
            V += vv * v3 / 6;
        }
        cout.precision(10);
        cout << fixed;
        cout << S << ' ' << V << '\n';
    }
}
