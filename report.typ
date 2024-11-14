#let title = [Assignment 12: Working with LLVM Abstract Transfer Functions]
#let date = [14 November 2024]

#set text(
    font: "Times New Roman",
    size: 11pt
)
#set page(
    paper: "us-letter",
    margin: 1in,
    header: context {
        if counter(page).get().first() > 1 [
            _
            Cayden Lund
            #h(1fr)
            #title
            #h(1fr)
            Page
            #counter(page).display(
                "1 / 1",
                both: true
            )
            _
        ]
    }
)

#align(center)[
    #text(22pt)[
        #title
    ]

    #text(15pt)[
        Cayden Lund

        #date

        #show link: underline
        Repository: #link("https://github.com/caydenlund/transfer-function-analysis")
    ]
]

The abstract domain that I picked was `ConstantRange`, and the transfer function that I used was `abs` (absolute value).
I chose this because it's simple enough to implement and test, while still being composed of multiple, simpler LLVM transfer functions.
My technique was to loop through all of the concrete values given by the concretization function over the abstract range, to insert the absolute value of the concrete value into a set, and then to abstractize that set.

Here are my results:

#align(center, block(table(
    columns: (auto, auto),
    align: (left, left),
    [Bitwidth], [10],
    [Total abstract values for this bitwidth], [523,777],
    [Tests where the built-in composite fn was more precise], [2,042],
    [Tests where my decomposed fn was more precise], [3],
    [Incomparable results], [0],
)))
