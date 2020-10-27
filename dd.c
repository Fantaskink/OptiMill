    struct Windmill Siemens;

    Siemens.price = 18894090;
    Siemens.height = 155;
    Siemens.wing_span = 130;
    Siemens.kWh = 4000;


    struct Area Omraade1;

    strcpy( Omraade1.name, "København");
    Omraade1.wind_speed = 4;
    Omraade1.region = 0;
    Omraade1.in_sea = 0;
    Omraade1.land_height = 2;
    Omraade1.tree_factor = 0;
    Omraade1.roughness = 1;
    Omraade1.dist_to_house = 120;
    Omraade1.dist_to_powergrid = 5;
    Omraade1.windmill = Vestas, Siemens;
    Omraade1.expenses = calc_total_expenses(Omraade1);


    struct Area area2;
   
    strcpy (area2.name, "Sydjylland");
    
    area2.wind_speed = 8;
    area2.region = 1;
    area2.in_sea = 0;
    area2.land_height = 2;
    area2.tree_factor = 0.2;
    area2.roughness = 0.4;
    area2.dist_to_house = 2000;
    area2.dist_to_powergrid = 5;
    area2.windmill = Vestas, Siemens;
    area2.expenses = calc_total_expenses(area2);


    struct Area area3;

    strcpy (area3.name, "Nordjylland");

    area3.wind_speed = 4;
    area3.region = 2;
    area3.in_sea = 0;
    area3.land_height = 2;
    area3.tree_factor = 0.1;
    area3.roughness = 0.4;
    area3.dist_to_house = 2000;
    area3.dist_to_powergrid = 5;
    area3.windmill = Vestas, Siemens;
    area3.expenses = calc_total_expenses(area3);


    struct Area area4;

    strcpy (area4.name, "Midtjylland");

    area4.wind_speed = 4;
    area4.region = 3;
    area4.in_sea = 0;
    area4.land_height = 2;
    area4.tree_factor = 0.2;
    area4.roughness = 0.4;
    area4.dist_to_house = 2000;
    area4.dist_to_powergrid = 5;
    area4.windmill = Vestas, Siemens;
    area4.expenses = calc_total_expenses(area4);


    struct Area area5;

    strcpy (area5.name, "Sjaeland");

    area5.wind_speed = 4;
    area5.region = 4;
    area5.in_sea = 0;
    area5.land_height = 2;
    area5.tree_factor = 0.3;
    area5.roughness = 0.4;
    area5.dist_to_house = 1000;
    area5.dist_to_powergrid = 5;
    area5.windmill = Vestas, Siemens;
    area5.expenses = calc_total_expenses(area5);
