import re
def main():
        int_codes = {}
        with open("ytab.h", 'r') as ytab:
                content = ytab.read()
                for word in content.split():
                        word = word.strip()
                        if re.match(r"^\d{3}$", word):
                                int_codes[word.strip()] = 0
        # int codes now contains all integer codes specified in ytab

        with open("scanner_output.txt", 'r') as f:
                for line in f.readlines():
                        line_split = line.split()
                        if re.match(r"^\d{3}$", line_split[0].strip()):
                                int_codes[line_split[0]] = 1

        found_count = 0
        total = len(int_codes)
        print("Integer codes not matched in testing:")
        for int_code, val in int_codes.items():
                if val == 0:
                        print(int_code)
                else:
                        found_count+=1

        print(f"Number of int codes found: {found_count}/{total}")

if __name__ == "__main__":
        main()