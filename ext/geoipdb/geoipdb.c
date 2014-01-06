#include "ruby.h"
#include "ipdb.h"

/**
Ruby Wrapper
*/

typedef struct geipdb {
  IPDB *db;
} geoipdb;

static VALUE cIpDb;

// free the memory used by the db, called by the Ruby-GC
void geoipdb_free(geoipdb *gi) {
  if(gi == NULL)
    return;
  if (gi->db != NULL){
    if(gi->db->cities != NULL ){
      free(gi->db->cities);
      gi->db->cities = NULL;
    }
    if(gi->db->ranges != NULL ){
      free(gi->db->ranges);
      gi->db->ranges = NULL;
    }
    if(gi->db != NULL){
      free(gi->db);
    }
  }
}


VALUE ipdb_init(VALUE self, VALUE cities_file_name, VALUE ranges_file_name, VALUE cache_file_name) {
  geoipdb *gi;

  Check_Type(cities_file_name, T_STRING);
  Check_Type(ranges_file_name, T_STRING);
  Check_Type(cache_file_name, T_STRING);

  char *cities_csv_file = RSTRING_PTR(cities_file_name);
  char *ranges_csv_file = RSTRING_PTR(ranges_file_name);
  char *cache_file =      RSTRING_PTR(cache_file_name);

  gi = ALLOC(geoipdb);

  gi->db= init_db(cities_csv_file, ranges_csv_file, cache_file);

  if(gi->db == NULL)
  {
    if(DEBUG)
      printf("Could not init DB!\n");
    /*
      TODO: Add geoipdb_free in this case.. though not important for production...
    */
    return Qnil;
  }else{
    if(DEBUG)
      printf("\nDB Init completed!\n");
    return(Data_Wrap_Struct(cIpDb, 0, geoipdb_free, gi));
  }
}


VALUE build_ip_information_object(IpRange *range, City *city, char* isp) {
  VALUE CIpInformation;

  CIpInformation = rb_const_get(rb_cObject, rb_intern("IpInformation"));

  VALUE ip_information = rb_funcall(CIpInformation, rb_intern("new"), 0);
  rb_ivar_set(ip_information, rb_intern("@country_iso_code"), rb_str_new2(city->country_iso2) );
  rb_ivar_set(ip_information, rb_intern("@city_name"), rb_str_new2(city->name) );
  rb_ivar_set(ip_information, rb_intern("@city_code"), INT2FIX(city->city_code) );
  rb_ivar_set(ip_information, rb_intern("@lng"), rb_float_new(city->lng) );
  rb_ivar_set(ip_information, rb_intern("@lat"), rb_float_new(city->lat) );
  rb_ivar_set(ip_information, rb_intern("@is_mobile"), range->is_mobile == 1 ? Qtrue : Qfalse );
  rb_ivar_set(ip_information, rb_intern("@isp_name"), isp == NULL || strcmp(isp, "?") == 0 ? Qnil : ID2SYM( rb_intern(isp) ) );

  return ip_information;
}

VALUE ipdb_information_for_ip(VALUE self, VALUE ip_string){
  char *ip = RSTRING_PTR(ip_string);
  geoipdb *gi;

  Data_Get_Struct(self, geoipdb, gi);

  IpRange* ip_range = find_range_for_ip(gi->db, ip);

  if(!ip_range)
    return Qnil;

  City * city = find_city_for_ip_range(gi->db, ip_range);
  if(!city)
    return Qnil;

  char* isp = find_isp_for_ip_range(gi->db, ip_range);

  return build_ip_information_object(ip_range, city, isp);
}

void Init_geoipdb(void)
{
  cIpDb = rb_define_class( "GeoIpDb", rb_cObject);
  rb_define_singleton_method( cIpDb, "init", ipdb_init, 3);
  rb_define_method( cIpDb, "information_for_ip",  ipdb_information_for_ip, 1);
}
