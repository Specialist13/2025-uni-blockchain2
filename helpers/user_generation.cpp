#include "user_generation.h"

std::string first_names[500] = {
    "James", "Mary", "John", "Patricia", "Robert", "Jennifer", "Michael", "Linda", "William", "Elizabeth",
    "David", "Barbara", "Richard", "Susan", "Joseph", "Jessica", "Thomas", "Sarah", "Charles", "Karen",
    "Christopher", "Nancy", "Daniel", "Lisa", "Matthew", "Betty", "Anthony", "Margaret", "Donald", "Sandra",
    "Mark", "Ashley", "Paul", "Kimberly", "Steven", "Emily", "Andrew", "Donna", "Kenneth", "Michelle",
    "Joshua", "Dorothy", "George", "Carol", "Kevin", "Amanda", "Brian", "Melissa", "Edward", "Deborah",
    "Ronald", "Stephanie", "Timothy", "Rebecca", "Jason", "Laura", "Jeffrey", "Sharon", "Ryan", "Cynthia",
    "Jacob", "Kathleen", "Gary", "Amy", "Nicholas", "Shirley", "Eric", "Angela", "Stephen", "Helen",
    "Jonathan", "Anna", "Larry", "Brenda", "Justin", "Pamela", "Scott", "Emma", "Brandon", "Nicole",
    "Benjamin", "Samantha", "Samuel", "Katherine", "Gregory", "Christine", "Frank", "Debra", "Alexander", "Rachel",
    "Raymond", "Catherine", "Patrick", "Carolyn", "Jack", "Janet", "Dennis", "Ruth", "Jerry", "Maria",
    "Tyler", "Heather", "Aaron", "Diane", "Jose", "Virginia", "Adam", "Julie", "Nathan", "Joyce",
    "Henry", "Victoria", "Douglas", "Olivia", "Zachary", "Kelly", "Peter", "Christina", "Kyle", "Lauren",
    "Walter", "Joan", "Ethan", "Evelyn", "Jeremy", "Judith", "Harold", "Andrea", "Keith", "Hannah",
    "Christian", "Megan", "Roger", "Cheryl", "Noah", "Jacqueline", "Gerald", "Martha", "Carl", "Madison",
    "Terry", "Teresa", "Sean", "Gloria", "Austin", "Sara", "Arthur", "Ann", "Lawrence", "Kathryn",
    "Jesse", "Abigail", "Dylan", "Sophia", "Bryan", "Frances", "Joe", "Jean", "Jordan", "Alice",
    "Billy", "Judy", "Bruce", "Isabella", "Ralph", "Julia", "Roy", "Grace", "Alan", "Amber",
    "Wayne", "Denise", "Eugene", "Danielle", "Juan", "Marilyn", "Gabriel", "Beverly", "Louis", "Charlotte",
    "Russell", "Theresa", "Randy", "Diana", "Vincent", "Natalie", "Philip", "Rose", "Logan", "Alexis",
    "Bobby", "Kayla", "Johnny", "Lori", "Howard", "Caroline", "Martin", "Courtney", "Ernest", "Avery",
    "Derek", "Hailey", "Victor", "Eleanor", "Chad", "Savannah", "Stanley", "Brooklyn", "Leonard", "Penelope",
    "Frederick", "Claire", "Dale", "Skylar", "Allen", "Lucy", "Shawn", "Paisley", "Clarence", "Everly"
};
std::string last_names[500] = {
    "Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis", "Rodriguez", "Martinez",
    "Hernandez", "Lopez", "Gonzalez", "Wilson", "Anderson", "Thomas", "Taylor", "Moore", "Jackson", "Martin",
    "Lee", "Perez", "Thompson", "White", "Harris", "Sanchez", "Clark", "Ramirez", "Lewis", "Robinson",
    "Walker", "Young", "Allen", "King", "Wright", "Scott", "Torres", "Nguyen", "Hill", "Flores",
    "Green", "Adams", "Nelson", "Baker", "Hall", "Rivera", "Campbell", "Mitchell", "Carter", "Roberts",
    "Gomez", "Phillips", "Evans", "Turner", "Diaz", "Parker", "Cruz", "Edwards", "Collins", "Reyes",
    "Stewart", "Morris", "Morales", "Murphy", "Cook", "Rogers", "Gutierrez", "Ortiz", "Morgan", "Cooper",
    "Peterson", "Bailey", "Reed", "Kelly", "Howard", "Ramos", "Kim", "Cox", "Ward", "Richardson",
    "Watson", "Brooks", "Chavez", "Wood", "James", "Bennett", "Gray", "Mendoza", "Ruiz", "Hughes",
    "Price", "Alvarez", "Castillo", "Sanders", "Patel", "Myers", "Long", "Ross", "Foster", "Jimenez",
    "Powell", "Jenkins", "Perry", "Russell", "Sullivan", "Bell", "Coleman", "Butler", "Henderson", "Barnes",
    "Gonzales", "Fisher", "Vasquez", "Simmons", "Romero", "Jordan", "Patterson", "Alexander", "Hamilton", "Graham",
    "Reynolds", "Griffin", "Wallace", "Moreno", "West", "Cole", "Hayes", "Bryant", "Herrera", "Gibson",
    "Ellis", "Tran", "Medina", "Aguilar", "Stevens", "Murray", "Ford", "Castro", "Marshall", "Owens",
    "Harrison", "Fernandez", "McDonald", "Woods", "Washington", "Kennedy", "Wells", "Vargas", "Henry", "Chen",
    "Freeman", "Webb", "Tucker", "Guzman", "Burns", "Crawford", "Olson", "Simpson", "Porter", "Hunter",
    "Gordon", "Mendez", "Silva", "Shaw", "Snyder", "Mason", "Dixon", "Munoz", "Hunt", "Hicks",
    "Holmes", "Palmer", "Wagner", "Black", "Robertson", "Boyd", "Rose", "Stone", "Salazar", "Fox",
    "Warren", "Mills", "Meyer", "Rice", "Schmidt", "Garza", "Daniels", "Ferguson", "Nichols", "Stephens",
    "Soto", "Weaver", "Ryan", "Gardner", "Payne", "Grant", "Dunn", "Kelley", "Spencer", "Hawkins",
    "Arnold", "Pierce", "Vazquez", "Hansen", "Peters", "Santos", "Hart", "Bradley", "Knight", "Elliott"
};

void generate_users(const std::string& filename, int num_users) {
    std::ofstream fout(filename);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> name_dist(0, 200);
    std::uniform_int_distribution<int> public_key_dist(0, 15);
    std::uniform_int_distribution<int> balance_dist(100, 1000000);

    for (int i = 0; i < num_users; ++i) {
        fout << std::left << std::setw(12) << first_names[name_dist(rng)] << " " << std::setw(12) << last_names[name_dist(rng)] << " ";
        for (int i = 0; i < 64; i++) fout << std::hex << public_key_dist(rng);
        fout << " " << std::dec << balance_dist(rng) << "\n";
    }
}