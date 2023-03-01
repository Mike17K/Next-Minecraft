'''
int dist(int t, int r, int x){
    return t - 4*r*r + 4*r - 2*r*x;
}
void dims(int t,int *x,int *y){
    int r = (int)(sqrt(t) + 1) / 2;
    if(r==0) {
        *x=0;
        *y=0;
        return
    }

    int p = (int)(t - (2*r - 1)*(2*r - 1)) / (2*r);

    
    if (p == 0){
        *x = -r + dist(t, r, p);
        *y = r;
    }
    else if(p == 1){
        *x = r;
        *y = r - dist(t, r, p);
    }
    else if( p == 2){
        *x = r - dist(t, r, p);
        *y = -r;
    }
    else if( p == 3){
        *x = -r;
        *y = -r + dist(t, r, p);
    }
}

'''

def dist(t, r, x):
    return int(t - 4*r**2 + 4*r - 2*r*x)

def dims(t):
    r = int((t**(0.5) + 1) / 2)
    if r == 0:
        return (0,0)
    
    p = int((t - (2*r - 1)**2) / (2*r))

    if p == 0:
        x = -r + dist(t, r, p)
        y = r
    elif p == 1:
        x = r
        y = r - dist(t, r, p)
    elif p == 2:
        x = r - dist(t, r, p)
        y = -r
    elif p == 3:
        x = -r
        y = -r + dist(t, r, p)

    return (x,y)

while True:
    t = int(input("Give a step t: "))

    x,y  =dims(t)
    print(f"({x}, {y})")
    print()

