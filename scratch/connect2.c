#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h> 



struct share {
    int x;
    int y;

};

/*
   int basis(int val, struct share ** shares, int numShares) {
   int numerator = shares[val]->y; 
   int j = 0;
   for (j; j < numShares; j++) {
   if (j != val) {

   }
   }

   }
   */ 

int main(char **args) {
    struct share ** shares =  malloc(sizeof(struct share*) * 30); 
    MYSQL_RES * result;
    MYSQL_RES * newres; 
    MYSQL_ROW row;
    MYSQL_ROW newrow; 
    MYSQL *connection, mysql;
    int state;
    /* connect to the MySQL database at localhost */
    mysql_init(&mysql);
    /*
     * dbstats.txt                                                                                                                                                               
     *  username:   n4q1a6kczuonf5l8
     *   Password    jsgsb3vqi3xvty8f
     *    ip 34.226.240.127
     *     db name ffj056kwbtsekz5v
     *
     */ 
    connection = mysql_real_connect(&mysql,"34.226.240.127", "n4q1a6kczuonf5l8", "jsgsb3vqi3xvty8f", 
            "ffj056kwbtsekz5v", 0, 0, 0);
    /* check for a connection error */
    if (connection == NULL) {
        /* print the error message */
        printf(mysql_error(&mysql));
        return 1;
    }
    int devid = 3; 
    //get active users for device. 
    char userQuery[50]; 
    memset(userQuery, 0, sizeof(userQuery)); 
    sprintf(userQuery, "select * from active_users where devid = %d", devid);  
    printf("userQuery is %s\n", userQuery); 

    state = mysql_query(connection, userQuery);  
    result = mysql_store_result(connection); 
    int num = 0;
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("row[0] %s, row[1] %s\n", row[0], row[1]); 
        int tempuid = (int)(row[0][0] - '0'); 

        char shareQuery[100]; 
        memset(shareQuery, 0, sizeof(shareQuery)); 
        sprintf(shareQuery, "select * from shares where uid = %d and devid = %d", tempuid, devid); 
        
        //old stuff 
        //state = mysql_query(connection,
         //       "select * from shares");
        state = mysql_query(connection, shareQuery); 
        if (state != 0) {
            printf(mysql_error(connection));
            return 1;
        }
        newres = mysql_store_result(connection); 
        printf("Rows: %d\n", mysql_num_rows(newres));
        /* process each row in the result set */
        while ((newrow = mysql_fetch_row(newres)) != NULL) {
            printf("Row is num %d\n", num);
            int max = 4;
            int i = 0;
            //shares at 2 and 3. 
            struct share * temp = malloc(sizeof(struct share));
            printf("newrow[2] is %s\n", newrow[2]);
            temp->x = (int)(newrow[2][0] - '0');
            temp->y = (int)(newrow[3][0] - '0');
            shares[num] = temp;

            printf("\n"); 
            num++;
        }
    }



    //mysql_free_result(result) 
    /*
       state = mysql_query(connection,
       "select * from shares");
       if (state != 0) {
       printf(mysql_error(connection));
       return 1;
       }
       */
    /* must call mysql_store_result( ) before you can issue 
     *       any other query calls */
    //result = mysql_store_result(connection);


    /* process each row in the result set */


    int i = 0; 
    int secret = 0;
    int value = 0; 
    int p = 11; 
    for (i = 0; i < num; i++) {
        int numerator = shares[i]->y; 
        int denom = 1; 
        for (int j = 0; j < num; j++) {
            if (i != j) {
                numerator *= (0 - shares[j]->x) % 11;  
                denom *= (shares[i]->x - shares[j]->x) % 11; 
                //printf("Numerator is %d, denom: %d\n", numerator, denom);  
            }
        }
        value = (numerator/denom) % 11; 
        //printf("Value is %d\n", value); 
        secret += value % 11; 
    }
    secret = secret % 11; 
    printf("Reconstructed secret is %d\n", secret); 

    //if works, post 1 to be update. 


    /*while ( ( row = mysql_fetch_row(result)) != NULL ) {
      printf("id: %s, name: %s\n",
      (row[0] ? row[0] : "NULL"),
      (row[1] ? row[1] : "NULL"));
      }*/
    /* free the result set */
    //mysql_free_result(result);
    /* close the connection */
    mysql_close(connection);
    //printf("Done.\n");
}
