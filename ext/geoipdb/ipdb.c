#include "ipdb.h"

#include <search.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

const char country_iso2_codes[251][3] = { "--","ad","ae","af","ag","ai","al","am","an",
  "ao","aq","ar","as","at","au","aw","az","ba","bb",
  "bd","be","bf","bg","bh","bi","bj","bm","bn","bo",
  "br","bs","bt","bv","bw","by","bz","ca","cc","cd",
  "cf","cg","ch","ci","ck","cl","cm","cn","co","cr",
  "cu","cv","cx","cy","cz","de","dj","dk","dm","do",
  "dz","ec","ee","eg","eh","er","es","et","fi","fj",
  "fk","fm","fo","fr","ga","gb","gd","ge","gf",
  "gh","gi","gl","gm","gn","gp","gq","gr","gs","gt",
  "gu","gw","gy","hk","hm","hn","hr","ht","hu","id",
  "ie","il","in","io","iq","ir","is","it","jm","jo",
  "jp","ke","kg","kh","ki","km","kn","kp","kr","kw",
  "ky","kz","la","lb","lc","li","lk","lr","ls","lt",
  "lu","lv","ly","ma","mc","md","mg","mh","mk","ml",
  "mm","mn","mo","mp","mq","mr","ms","mt","mu","mv",
  "mw","mx","my","mz","na","nc","ne","nf","ng","ni",
  "nl","no","np","nr","nu","nz","om","pa","pe","pf",
  "pg","ph","pk","pl","pm","pn","pr","ps","pt","pw",
  "py","qa","re","ro","ru","rw","sa","sb","sc","sd",
  "se","sg","sh","si","sj","sk","sl","sm","sn","so",
  "sr","st","sv","sy","sz","tc","td","tf","tg","th",
  "tj","tk","tm","tn","to","tl","tr","tt","tv","tw",
  "tz","ua","ug","um","us","uy","uz","va","vc","ve",
  "vg","vi","vn","vu","wf","ws","ye","yt","rs","za",
  "zm","me","zw","ax","gg","im","je",
  "bl","mf","bq","cw","ss","sx"};

static const unsigned num_countries = (unsigned)(sizeof(country_iso2_codes)/sizeof(country_iso2_codes[0]));

const char country_iso3_codes[251][4] = { "--","and","are","afg","atg","aia","alb","arm","ant",
  "ago","ata","arg","asm","aut","aus","abw","aze","bih","brb",
  "bgd","bel","bfa","bgr","bhr","bdi","ben","bmu","brn","bol",
  "bra","bhs","btn","bvt","bwa","blr","blz","can","cck","cod",
  "caf","cog","che","civ","cok","chl","cmr","chn","col","cri",
  "cub","cpv","cxr","cyp","cze","deu","dji","dnk","dma","dom",
  "dza","ecu","est","egy","esh","eri","esp","eth","fin","fji",
  "flk","fsm","fro","fra","gab","gbr","grd","geo","guf",
  "gha","gib","grl","gmb","gin","glp","gnq","grc","sgs","gtm",
  "gum","gnb","guy","hkg","hmd","hnd","hrv","hti","hun","idn",
  "irl","isr","ind","iot","irq","irn","isl","ita","jam","jor",
  "jpn","ken","kgz","khm","kir","com","kna","prk","kor","kwt",
  "cym","kaz","lao","lbn","lca","lie","lka","lbr","lso","ltu",
  "lux","lva","lby","mar","mco","mda","mdg","mhl","mkd","mli",
  "mmr","mng","mac","mnp","mtq","mrt","msr","mlt","mus","mdv",
  "mwi","mex","mys","moz","nam","ncl","ner","nfk","nga","nic",
  "nld","nor","npl","nru","niu","nzl","omn","pan","per","pyf",
  "png","phl","pak","pol","spm","pcn","pri","pse","prt","plw",
  "pry","qat","reu","rou","rus","rwa","sau","slb","syc","sdn",
  "swe","sgp","shn","svn","sjm","svk","sle","smr","sen","som",
  "sur","stp","slv","syr","swz","tca","tcd","atf","tgo","tha",
  "tjk","tkl","tkm","tun","ton","tls","tur","tto","tuv","twn",
  "tza","ukr","uga","umi","usa","ury","uzb","vat","vct","ven",
  "vgb","vir","vnm","vut","wlf","wsm","yem","myt","srb","zaf",
  "zmb","mne","zwe","ala","ggy","imn","jey",
  "blm","maf","bes","cuw","ssd","sxm"};


void
print_range(const IpRange* e){
  printf( "from: %lu, to:%lu ->City-idx: %i \n",e->from, e->to,e->city_index );
}

void
print_ranges(IPDB * db){
  int i;
  for(i = 0; i < db->ranges_count; ++i)
  {
    print_range(&(db->ranges[i]));
  }
}

void
print_city(const City * e){
  if(e == NULL)
  {
    return;
  }
  printf( "City: code:%i, name:%s, country: %s, lat: %10.7f, lng: %10.7f  \n",e->city_code, e->name, e->country_iso3, e->lat, e->lng );
}

void
print_cities(IPDB * db){
  int i;
  for(i = 0; i < db->cities_count; ++i)
  {
    print_city(&(db->cities[i]));
  }
}

void print_stats(IPDB * db){
  printf("DB STATS: \n");
  printf("\tCities: %i\n", db->cities_count);
  printf("\tRanges: %i\n", db->ranges_count);
}

double
get_time(struct timeval *tim){
  gettimeofday(tim, NULL);
  return tim->tv_sec+(tim->tv_usec/1000000.0);
}


unsigned long
ip_to_int(const char *addr){
  unsigned int    c, octet, t;
  unsigned long   ipnum;
  int             i = 3;

  octet = ipnum = 0;
  while ((c = *addr++)) {
    if (c == '.') {
      if (octet > 255)
        return 0;
      ipnum <<= 8;
      ipnum += octet;
      i--;
      octet = 0;
    } else {
      t = octet;
      octet <<= 3;
      octet += t;
      octet += t;
      c -= '0';
      if (c > 9)
        return 0;
      octet += c;
    }
  }
  if ((octet > 255) || (i != 0))
    return 0;
  ipnum <<= 8;
  return ipnum + octet;
}


unsigned char con_type_to_int(char* con_type) {
  // possible values
  // ?
  // dialup
  // broadband
  // cable
  // xdsl
  // mobile
  // t1
  // t3
  // oc3
  // oc12
  // satellite
  // wireless
  if(strlen(con_type) > 0 && (con_type[0] == 'm'))
    return 1;
  return 0;
}

// Function to compare
// - either two ip-ranges: i.e.: a(from...to) <=> b(from...to)
// - or a ip(i.e. range without to) and an ip-range: i.e. a(from...NULL) <=> b(from...to); a(from...to) <=> b(from ... NULL)
int compare_ranges(const void *fa, const void *fb) {
  if(fa == NULL)
  {
    if(DEBUG){printf("FA IS NULL!!!\n");}
    return 0;
  }
  if(fb == NULL)
  {
    if(DEBUG){printf("FB IS NULL!!!\n");}
    return 0;
  }


  const IpRange *a = (IpRange *) fa;
  const IpRange *b = (IpRange *) fb;

  if(a->from>0 && a->to>0 && b->from>0 && b->to>0){ //regular case: both entries are ranges
    if(a->to  <  b->from)    {
      return -1;
    }else if(a->from > b->to){
      return +1;
    }else{
      return 0;
    }
  }else if(a->to == 0 && b->to>0){//a is a search_object
    if(a->from  <  b->from)    {
      return -1;
    }else if(a->from > b->to){
      return +1;
    }else{
      return 0;
    }
  }else if(b->to == 0 && a->to>0){//b is a search_object
    if(b->from  <  a->from){
      return -1;
    }else if(b->from > a->to){
      return +1;
    }else{
      return 0;
    }
  }else if(a->to == 0 && b->to == 0){ //both are search objects - this should not happen!
    return a->from - b->from;
  }
  return 0;
}


int
compare_cities(const void *a, const void *b){
  const City city_a = *(City*)a;
  const City city_b = * (City*) b;
  // sort cities by city_code
  return city_a.city_code - city_b.city_code;
}

void
sort_cities(IPDB * db){
  if(DEBUG)
    printf("Sorting %i Cities in db...\n", db->cities_count);

  struct timeval tim;
  double t1 = get_time(&tim);

  qsort(db->cities,db->cities_count,sizeof(City), compare_cities);
  if(DEBUG)
    printf("\n Sorting cities needed %.6lf seconds\n", get_time(&tim)-t1);
}


int // returns a city-index
city_index_by_code(IPDB * db, int city_code){
  City *search, *result;
  search = malloc(sizeof(City));
  search->city_code = city_code;
  result = (City*) bsearch(search, db->cities, db->cities_count, sizeof(City), compare_cities);

  if(search != NULL)
    free(search);

  if(result == NULL)
  {
    if(DEBUG)
      printf("Could not find searched city with code: %i \n", city_code);
    return -1;
  }else{
    int index;
    index = (result - db->cities);
    return index;
  }
}


IpRange* find_range_for_ip(IPDB *db, char *ip) {
  IpRange* search;
  IpRange* result;
  search = (IpRange *)malloc(sizeof(IpRange));

  if(db == NULL)
  {
    if(DEBUG){printf("ERROR: DB ist NULL! \n");}
    return NULL;
  }

  if(db->ranges_count == 0)
  {
    if(DEBUG){printf("ERROR: DB has no Ranges Data. Can not search!\n");}
    return NULL;
  }

  search->from = ip_to_int(ip);
  search->to=0;
  search->city_index = 0;
  if(DEBUG)
    printf("Searching for: ip=%s, ipnum=%lu \n", ip, search->from);
  result = (IpRange*)bsearch(search, db->ranges, db->ranges_count, sizeof(IpRange), compare_ranges);
  if(search != NULL)
    free(search);

  if(result == NULL)
  {
    if(DEBUG)
      printf("ERROR: Could not find the IP: %s! THIS SHOULD NOT HAPPEN!\n", ip);
    return NULL;
  } else {
    if(DEBUG) {
      printf("Found Range: \t");
      print_range(result);
    }
    return (IpRange*)result;
  }
}

City * find_city_for_ip_range(IPDB * db, IpRange* range)
{
  if(!db || !range)
    return NULL;

  if(db->cities_count == 0)
  {
    if(DEBUG)
      printf("ERROR: DB has no City Data. Can not search!\n");
    return NULL;
  }

  if( range->city_index <= 0 || range->city_index >= db->cities_count )
  {
    if(DEBUG)
      printf("ERROR: Could not find city with index: %i - THIS SHOULD NOT HAPPEN!\n", range->city_index);
  }

  return &(db->cities[range->city_index]);
}

char* find_isp_for_ip_range(IPDB * db, IpRange* range)
{
  if( range == NULL || range->isp_index < 0){
    if(DEBUG){printf("Could not find isp for isp_index=%i\n", range->isp_index);}
    return NULL;
  }
  return db->isps[range->isp_index];
}

int16
isp_index_by_name(IPDB * db, char* isp_name){
  if(isp_name == NULL || isp_name == "")
    return -1;
  if( db->isps_count > 0){
    int16 i = 0;
    for( i = 0; i < db->isps_count; i++)
    {
      if( strcmp(db->isps[i], isp_name)==0)
      {
        return i;
      }
    }
  }
  // add new isp
  if(db->isps_count < MAX_ISPS_COUNT){
    int16 new_index = db->isps_count;
    strncpy(db->isps[new_index], isp_name, MAX_ISP_NAME_LENGTH);
    db->isps_count++;
    return new_index;
  }else{
    if(DEBUG){printf("ERROR: MAX_ISPS_COUNT = %i limit reached - this should not happen!\n", MAX_ISPS_COUNT);}
    return -1;
  }
}



// read ip-ranges from csv file, of format:
// from_ip|to_ip|contype|city_code
void
read_ranges_csv(IPDB * db){
  struct timeval tim;
  double t1 = get_time(&tim);

  db->ranges = malloc(sizeof(IpRange) * db->max_ranges_count);

  if(DEBUG)
    printf("Parsing RANGES-CSV-file: %s\n", db->ranges_csv_file);
  FILE * f = fopen(db->ranges_csv_file, "rt");
  if(f == NULL)
  {
    if(DEBUG)
      printf("Could not open the CSV-file: %s\n", db->ranges_csv_file);
    return;
  }
  char line[256];
  char* from;
  char* to;
  char* city_code;
  int city_index;

  char* con_type;
  char* isp_name;
  uint16 isp_index;

  int invalid_cities_count = 0;

  IpRange* entry;
  db->ranges_count = 0;
  while (fgets(line, sizeof(line) ,f) && db->ranges_count < db->max_ranges_count){
    from = NULL;
    to = NULL;
    city_code = NULL;
    city_index = 0;

    con_type = NULL;
    isp_name = NULL;
    int16 isp_index = -1;

    if(DEBUG && db->ranges_count % 1000000 == 0)
      printf("Worked lines: %i\n", db->ranges_count);

    from =      strtok(line, RANGES_DELIM);
    to =        strtok(NULL, RANGES_DELIM);
    con_type =  strtok(NULL, RANGES_DELIM);
    city_code = strtok(NULL, RANGES_DELIM);
    isp_name =  strtok(NULL, RANGES_DELIM);

    city_index = city_index_by_code(db, atoi(city_code));
    isp_index = isp_index_by_name(db, isp_name);

    if(city_index < 0)
    {
      if(DEBUG)
        printf("Could not find city for code: %i\n", atoi(city_code));
      invalid_cities_count ++;
      continue;
    }else{
      entry = &(db->ranges[db->ranges_count]);
      entry->from = ip_to_int(from);
      entry->to = ip_to_int(to);
      entry->is_mobile = con_type_to_int(con_type);
      entry->city_index = city_index;
      entry->isp_index = isp_index;

      db->ranges_count++;
    }
  }
  if(DEBUG)
  {
    if(invalid_cities_count ){printf("Found invalid cities: %i\n", invalid_cities_count);}
    printf("\n Parsing of %i records needed %.6lf seconds\n", db->ranges_count, get_time(&tim)-t1);
  }
}



//translate country iso3 to iso2
char *
iso2_code(char* iso3){
  int i = 0;
  for( i = 0; i < num_countries; i++)
  {
    if(  strcmp(country_iso3_codes[i],iso3)==0)
    {
      return (char*) country_iso2_codes[i];
    }
  }
  return (char*) country_iso2_codes[0];
}

//read city-data from csv-file of format:
// COUNTRY,REGION,CITY-NAME,METRO-CODE,CITY-CODE,LATITUDE,LONGITUDE
void
read_cities_csv(IPDB * db){
  struct timeval tim;
  double t1 = get_time(&tim);

  db->cities_count = 0;
  db->cities = malloc(sizeof(City) * db->max_cities_count);

  if(DEBUG)
    printf("Parsing Cities-CSV-file: %s\n", db->cities_csv_file);
  FILE * f = fopen(db->cities_csv_file, "rt");
  if(f == NULL)
  {
    if(DEBUG)
      printf("Could not open the Cities-CSV-file: %s\n", db->cities_csv_file);
    return;
  }
  char line[256];
  char  *country, *region, *name,*metro_code,*city_code,*lat,*lng ;
  int i = 0;
  City* entry;

  while (fgets(line,sizeof(line),f) && db->cities_count < db->max_cities_count){
    i++;
    if(i == 1)
      continue;//skip the header

    if(DEBUG && i % 1000000 == 0)
    {
      printf("Worked lines: %i\n", i);
    }
    // COUNTRY,REGION,CITY-NAME,METRO-CODE,CITY-CODE,LATITUDE,LONGITUDE
    country =    strtok(line, CITIES_DELIM);
    region =     strtok(NULL, CITIES_DELIM);
    name =       strtok(NULL, CITIES_DELIM);
    metro_code = strtok(NULL, CITIES_DELIM);
    city_code =  strtok(NULL, CITIES_DELIM);
    lat =        strtok(NULL, CITIES_DELIM);
    lng =        strtok(NULL, CITIES_DELIM);

    entry = &(db->cities[db->cities_count]);

    strncpy(entry->country_iso3, country, strlen(country));

    strncpy(entry->country_iso2, iso2_code(country), 2);
    strncpy(entry->name, name, strlen(name));

    entry->city_code =    atoi(city_code);
    entry->lat =          atof(lat);
    entry->lng =          atof(lng);
    db->cities_count++;
  }
  if(DEBUG)
    printf("\n Parsing of %i records needed %.6lf seconds\n", db->cities_count, get_time(&tim)-t1);
}

/**
  cache-file is an exact binary copy of the ranges+cities-arrays from memory,
  the layout goes like this:
  db->cities_count [4 Bytes]
  db->ranges_count [4 Bytes]

  db->cities [sizeof(City)=24 x db->ranges_count Bytes]
  db->ranges [sizeof(IpRange)=24 x db->ranges_count Bytes]
  */
void
write_cache_file(IPDB * db){
  struct timeval tim;
  double t1 = get_time(&tim);
  int objects_written;

  FILE * f;
  f = fopen(db->cache_file_name, "w");
  if(f==NULL){
    if(DEBUG)
      printf("Could not open Cache-File: %s\n", db->cache_file_name);
    return;
  }
  if(DEBUG){
    printf("Dumping %i records to cache-file: %s\n\n", db->ranges_count, db->cache_file_name);

    //write the record length at file header
    printf("Writing DB-Header of length: %li\n",sizeof(db->ranges_count));

    printf("RecordLength: %li\n",sizeof(IpRange));
    printf("FieldLength: %li\n",sizeof(db->ranges[0].from));
  }
  //write the header: i.e.: numbers of records
  fwrite(&(db->cities_count), sizeof(db->cities_count),1,f);
  fwrite(&(db->isps_count), sizeof(db->isps_count),1,f);
  fwrite(&(db->ranges_count), sizeof(db->ranges_count),1,f);

  if(DEBUG)
    printf("Writing Contents with %i cities, a %li bytes each, should = %li \n", db->cities_count, sizeof(City), db->cities_count * sizeof(City));
  //write the actual data: all the ranges-array-buffer:
  objects_written = fwrite(db->cities, sizeof(City), db->cities_count, f);

  if(DEBUG)
    printf("Writing Contents with %i isps, a %i bytes each, should = %i \n", db->isps_count, MAX_ISP_NAME_LENGTH, db->isps_count * MAX_ISP_NAME_LENGTH);
  //write the actual data: all the ranges-array-buffer:
  objects_written += fwrite(db->isps, MAX_ISP_NAME_LENGTH, db->isps_count, f);

  if(DEBUG)
    printf("Writing Contents with %i ranges, a %li bytes each, should = %li \n", db->ranges_count, sizeof(IpRange), db->ranges_count * sizeof(IpRange));
  //write the actual data: all the ranges-array-buffer:
  objects_written += fwrite(db->ranges, sizeof(IpRange), db->ranges_count, f);


  fclose(f);
  if(DEBUG)
    printf("\n Writing CacheFile of %i objects needed %.6lf seconds\n", objects_written, get_time(&tim)-t1);
}

int
read_cache_file(IPDB * db){
  struct timeval tim;
  double t1 = get_time(&tim);
  FILE * f;
  f = fopen(db->cache_file_name, "r");
  if(f==NULL){
    if(DEBUG)
      printf("Could not open Cache-File: %s\n", db->cache_file_name);
    return 0;
  }
  int cities_header_read = fread(&(db->cities_count), sizeof(db->cities_count),1,f);
  int isps_header_read = fread(&(db->isps_count), sizeof(db->isps_count),1,f);
  int ranges_header_read = fread(&(db->ranges_count), sizeof(db->ranges_count),1,f);


  if(cities_header_read == 0 || isps_header_read == 0 || ranges_header_read == 0 || db->cities_count == 0 || db->isps_count ==0 || db->ranges_count ==0)
  {
    if(DEBUG){printf("Could not read Cities-Header from Cache-File: %s\n", db->cache_file_name);}
    return 0;
  }
  if(DEBUG)
    printf("Reading DB-Header from Cache-File: %s, with %i cities, %iisps and %i ranges\n",db->cache_file_name, db->cities_count, db->isps_count, db->ranges_count);

  int objects_read = 0;
  if(DEBUG)
    printf("Allocating: %lu for cities-array \n", sizeof(City)*(db->cities_count));
  db->cities = malloc(sizeof(City) * db->cities_count);
  objects_read += fread(db->cities, sizeof(City),db->cities_count,f);

  if(DEBUG)
    printf("Reading in the isps into preallocated buffer of size: %lu\n", sizeof(db->isps));
  objects_read += fread(db->isps, MAX_ISP_NAME_LENGTH, db->isps_count,f);

  if(DEBUG)
    printf("Allocating: %lu for ranges-array \n", sizeof(IpRange)*(db->ranges_count));
  db->ranges = malloc(sizeof(IpRange) * db->ranges_count);
  objects_read += fread(db->ranges, sizeof(IpRange),db->ranges_count,f);


  fclose(f);
  if(DEBUG)
    printf("Reading cacheFile of %i objects needed %.6lf seconds\n", objects_read, get_time(&tim)-t1);
  return objects_read;
}

void
benchmark_search(IPDB * db,int count){
  printf("(Naiv) benchmark of the City-Search-Function with %i counts \n", count);
  struct timeval tim;
  double t1 = get_time(&tim);
  int i;
  City * city;

  for(i=0;i<count; i++){
    IpRange* range = find_range_for_ip(db,"278.50.47.0");
    City* city = find_city_for_ip_range(db,range);
  }
  double delta = get_time(&tim)-t1;

  printf("\n\nSearch: %.6lf seconds elapsed, i.e. %.6lf Ops/Second \n", delta, count / delta);
}

IPDB * init_db(char * cities_csv_file, char * ranges_csv_file, char * cache_file_name){
  if(DEBUG)
    printf("Initializing db\n");
  IPDB *db;
  db = (IPDB*)malloc(sizeof(IPDB));
  if (db == NULL) //no memory left
    return NULL;
  db->cities = NULL;
  db->ranges = NULL;
  db->cache_file_name = cache_file_name;

  db->cities_csv_file = cities_csv_file;
  db->max_cities_count = MAX_CITIES_COUNT;
  db->ranges_csv_file = ranges_csv_file;
  db->max_ranges_count = MAX_RANGES_COUNT;

  db->isps_count = 0;

  if(USE_CACHE && read_cache_file(db) > 0){
    if(DEBUG)
      printf("Loaded DB from Cache-File with %i records \n", db->ranges_count);
  }else{
    if(DEBUG)
      printf("Initializing IPDB from CSV-file: %s \n", db->ranges_csv_file);
    read_cities_csv(db);
    if(db->cities_count == 0)
    {
      return NULL;
    }
    sort_cities(db);
    read_ranges_csv(db);
    if(db!=NULL && db->ranges_count > 0 && USE_CACHE)
    {
      if(DEBUG)
        printf("Got %i records from CSV-file, writing to cache...\n", db->ranges_count);
      write_cache_file(db);
    }
  }
  return db;
}

