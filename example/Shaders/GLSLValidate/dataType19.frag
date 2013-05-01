uniform sampler1D s;
void main()
{
    int i = int(s); // conversion not allowed
}
