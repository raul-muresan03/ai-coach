import { Header } from "@/components/ui/header";

export default function Home() {
    return (
        <div>
            <Header />
            <div className="h-screen">
                <div className="flex flex-col items-center justify-center text-6xl font-bold">
                    Pick one method
                </div>


                <div className="flex justify-between text-4xl font-bold">
                    <div>

                    </div>
                    <button>
                        Classic bodyfat calculator
                    </button>
                    <button>
                        Bodyfat predictor with image segmentation
                    </button>
                </div>
            </div>
        </div>
    )
}